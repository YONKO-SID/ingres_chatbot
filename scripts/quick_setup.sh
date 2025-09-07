#!/bin/bash
# INGRES ChatBot - Quick Database Setup Script
# Run this script to set up the complete real-time data system

set -e  # Exit on any error

echo "🌊 INGRES ChatBot - Real-time Data Setup"
echo "========================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
if [[ $EUID -eq 0 ]]; then
   print_error "This script should not be run as root"
   exit 1
fi

# Create necessary directories
print_status "Creating project directories..."
mkdir -p logs data/raw_downloads data/processed scripts/backup

# Check system requirements
print_status "Checking system requirements..."

# Check Python
if ! command -v python3 &> /dev/null; then
    print_error "Python 3 is required but not installed"
    exit 1
fi

# Check PostgreSQL
if ! command -v psql &> /dev/null; then
    print_warning "PostgreSQL client not found. Installing..."
    
    # Detect OS and install PostgreSQL
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Ubuntu/Debian
        if command -v apt-get &> /dev/null; then
            sudo apt-get update
            sudo apt-get install -y postgresql postgresql-contrib python3-pip
        # CentOS/RHEL
        elif command -v yum &> /dev/null; then
            sudo yum install -y postgresql postgresql-server python3-pip
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if command -v brew &> /dev/null; then
            brew install postgresql python3
        else
            print_error "Homebrew not found. Please install PostgreSQL manually"
            exit 1
        fi
    fi
fi

# Install Python dependencies
print_status "Installing Python dependencies..."
pip3 install --user requests pandas psycopg2-binary openpyxl sqlalchemy python-dotenv

# Set up environment variables
print_status "Setting up environment configuration..."
cat > .env << EOF
# Database Configuration
DB_HOST=localhost
DB_NAME=ingres_groundwater
DB_USER=postgres
DB_PASSWORD=ingres_password
DB_PORT=5432

# API Configuration
CGWB_API_KEY=
NWIC_API_KEY=
API_RATE_LIMIT=100

# Logging
LOG_LEVEL=INFO
LOG_FILE=logs/ingres_data.log
EOF

print_success "Environment file created: .env"

# Database setup
print_status "Setting up PostgreSQL database..."

# Check if PostgreSQL is running
if ! pgrep -x "postgres" > /dev/null; then
    print_status "Starting PostgreSQL service..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        sudo systemctl start postgresql
        sudo systemctl enable postgresql
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        brew services start postgresql
    fi
fi

# Create database and user
print_status "Creating database and user..."
sudo -u postgres psql << EOF
-- Create database
DROP DATABASE IF EXISTS ingres_groundwater;
CREATE DATABASE ingres_groundwater;

-- Create user
DROP USER IF EXISTS ingres_user;
CREATE USER ingres_user WITH PASSWORD 'ingres_password';

-- Grant permissions
GRANT ALL PRIVILEGES ON DATABASE ingres_groundwater TO ingres_user;

-- Connect to database and grant schema permissions
\c ingres_groundwater;
GRANT ALL ON SCHEMA public TO ingres_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO ingres_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO ingres_user;

\q
EOF

# Run database schema setup
print_status "Setting up database schema..."
psql -h localhost -U ingres_user -d ingres_groundwater -f scripts/setup_database.sql

print_success "Database schema created successfully"

# Test database connection
print_status "Testing database connection..."
python3 << EOF
import psycopg2
try:
    conn = psycopg2.connect(
        host='localhost',
        database='ingres_groundwater',
        user='ingres_user',
        password='ingres_password'
    )
    cursor = conn.cursor()
    cursor.execute('SELECT COUNT(*) FROM groundwater_data;')
    count = cursor.fetchone()[0]
    print(f"✅ Database connection successful. {count} records found.")
    conn.close()
except Exception as e:
    print(f"❌ Database connection failed: {e}")
    exit(1)
EOF

# Make scripts executable
print_status "Setting up executable scripts..."
chmod +x scripts/*.py
chmod +x scripts/*.sh

# Create cron job for automatic updates
print_status "Setting up automatic data updates..."
cat > scripts/update_cron.sh << 'EOF'
#!/bin/bash
# Automatic data update script
cd "$(dirname "$0")/.."
python3 scripts/fetch_cgwb_data.py >> logs/cron_updates.log 2>&1
EOF

chmod +x scripts/update_cron.sh

# Add to crontab (optional)
read -p "Do you want to set up automatic data updates every 6 hours? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    # Add cron job
    (crontab -l 2>/dev/null; echo "0 */6 * * * $(pwd)/scripts/update_cron.sh") | crontab -
    print_success "Automatic updates scheduled every 6 hours"
fi

# Initial data fetch
print_status "Performing initial data fetch..."
python3 scripts/fetch_cgwb_data.py

# Verify data was loaded
print_status "Verifying data load..."
python3 << EOF
import psycopg2
try:
    conn = psycopg2.connect(
        host='localhost',
        database='ingres_groundwater',
        user='ingres_user',
        password='ingres_password'
    )
    cursor = conn.cursor()
    
    # Check total records
    cursor.execute('SELECT COUNT(*) FROM groundwater_data;')
    total_count = cursor.fetchone()[0]
    
    # Check by category
    cursor.execute("""
        SELECT category, COUNT(*) 
        FROM groundwater_data 
        GROUP BY category 
        ORDER BY COUNT(*) DESC;
    """)
    categories = cursor.fetchall()
    
    print(f"📊 Total records: {total_count}")
    print("📈 By category:")
    for cat, count in categories:
        print(f"   {cat}: {count}")
    
    # Check latest data
    cursor.execute("""
        SELECT MAX(last_updated), MAX(assessment_year) 
        FROM groundwater_data;
    """)
    last_update, latest_year = cursor.fetchone()
    print(f"🕒 Last updated: {last_update}")
    print(f"📅 Latest assessment year: {latest_year}")
    
    conn.close()
    
except Exception as e:
    print(f"❌ Data verification failed: {e}")
EOF

# Create backup script
print_status "Creating backup script..."
cat > scripts/backup_database.sh << 'EOF'
#!/bin/bash
# Database backup script
BACKUP_DIR="data/backups"
mkdir -p "$BACKUP_DIR"

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
BACKUP_FILE="$BACKUP_DIR/ingres_groundwater_$TIMESTAMP.sql"

pg_dump -h localhost -U ingres_user ingres_groundwater > "$BACKUP_FILE"

# Compress backup
gzip "$BACKUP_FILE"

# Keep only last 7 days of backups
find "$BACKUP_DIR" -name "*.sql.gz" -mtime +7 -delete

echo "Backup completed: ${BACKUP_FILE}.gz"
EOF

chmod +x scripts/backup_database.sh

# Create monitoring script
print_status "Creating monitoring script..."
cat > scripts/monitor_data.py << 'EOF'
#!/usr/bin/env python3
"""Monitor data freshness and quality"""

import psycopg2
from datetime import datetime, timedelta
import os

def check_data_freshness():
    """Check if data is up to date"""
    try:
        conn = psycopg2.connect(
            host='localhost',
            database='ingres_groundwater',
            user='ingres_user',
            password='ingres_password'
        )
        cursor = conn.cursor()
        
        # Check last update time
        cursor.execute("SELECT MAX(last_updated) FROM groundwater_data;")
        last_update = cursor.fetchone()[0]
        
        if last_update:
            hours_old = (datetime.now() - last_update).total_seconds() / 3600
            if hours_old > 24:
                print(f"⚠️  Data is {hours_old:.1f} hours old")
            else:
                print(f"✅ Data is fresh ({hours_old:.1f} hours old)")
        else:
            print("❌ No data found")
        
        # Check data quality
        cursor.execute("""
            SELECT 
                COUNT(*) as total,
                COUNT(CASE WHEN category IS NULL THEN 1 END) as missing_category,
                COUNT(CASE WHEN stage_of_extraction = 0 THEN 1 END) as zero_extraction
            FROM groundwater_data;
        """)
        
        total, missing_cat, zero_ext = cursor.fetchone()
        print(f"📊 Total records: {total}")
        print(f"🔍 Missing categories: {missing_cat}")
        print(f"⚠️  Zero extraction: {zero_ext}")
        
        conn.close()
        
    except Exception as e:
        print(f"❌ Monitoring failed: {e}")

if __name__ == "__main__":
    check_data_freshness()
EOF

chmod +x scripts/monitor_data.py

# Final setup verification
print_status "Running final verification..."

# Test all components
echo "🧪 Testing all components..."

# Test database
python3 scripts/monitor_data.py

# Test data fetcher
print_status "Testing data fetcher (dry run)..."
python3 -c "
from scripts.fetch_cgwb_data import CGWBDataFetcher
fetcher = CGWBDataFetcher()
print('✅ Data fetcher initialized successfully')
"

print_success "🎉 Setup completed successfully!"
echo
echo "📋 Next Steps:"
echo "1. Update .env file with actual API keys if available"
echo "2. Run 'python3 scripts/fetch_cgwb_data.py' to fetch latest data"
echo "3. Run 'python3 scripts/monitor_data.py' to check data status"
echo "4. Set up your C application to connect to the database"
echo
echo "📁 Important Files:"
echo "   - Database: ingres_groundwater (PostgreSQL)"
echo "   - Config: .env"
echo "   - Logs: logs/"
echo "   - Scripts: scripts/"
echo
echo "🔧 Database Connection:"
echo "   Host: localhost"
echo "   Database: ingres_groundwater"
echo "   User: ingres_user"
echo "   Password: ingres_password"
echo
print_success "INGRES ChatBot real-time data system is ready! 🚀"
# 🌊 Real-Time Groundwater Data Sources & Integration Guide

**For Ram (Database Team) - Complete Implementation Guide**

## 🎯 Official Government Data Sources

### 1. **Central Ground Water Board (CGWB) - Primary Source**

#### **Direct API Access**
```bash
# CGWB Water Level Data API
BASE_URL="http://cgwb.gov.in/webgis/services"

# Get state-wise groundwater data
curl "http://cgwb.gov.in/webgis/services/groundwater/MapServer/0/query?where=1%3D1&outFields=*&f=json"

# Get district-wise data
curl "http://cgwb.gov.in/webgis/services/groundwater/MapServer/1/query?where=STATE_NAME='PUNJAB'&outFields=*&f=json"

# Get block-wise assessment data
curl "http://cgwb.gov.in/webgis/services/groundwater/MapServer/2/query?where=DISTRICT='AMRITSAR'&outFields=*&f=json"
```

#### **CGWB Assessment Reports (Excel/CSV Downloads)**
```bash
# Annual groundwater assessment reports
wget "http://cgwb.gov.in/documents/2023-groundwater-assessment.xlsx"
wget "http://cgwb.gov.in/documents/state-wise-assessment-2023.csv"

# Water level monitoring data
wget "http://cgwb.gov.in/monitoring/water-level-data-2023.xlsx"
```

### 2. **National Water Informatics Centre (NWIC)**

#### **India-WRIS Portal API**
```bash
# Base URL for India Water Resources Information System
BASE_URL="http://www.india-wris.nrsc.gov.in/api"

# Groundwater data endpoints
curl "http://www.india-wris.nrsc.gov.in/api/groundwater/state/PUNJAB"
curl "http://www.india-wris.nrsc.gov.in/api/groundwater/district/AMRITSAR"
curl "http://www.india-wris.nrsc.gov.in/api/monitoring/wells/realtime"
```

#### **NWIC Data Portal Downloads**
```bash
# Download comprehensive datasets
wget "http://www.india-wris.nrsc.gov.in/downloads/groundwater-assessment-2023.zip"
wget "http://www.india-wris.nrsc.gov.in/downloads/water-level-monitoring.csv"
```

### 3. **Bhuvan Geoportal (ISRO)**

#### **Bhuvan API for Spatial Data**
```bash
# ISRO's Bhuvan portal for geospatial groundwater data
BASE_URL="https://bhuvan-app1.nrsc.gov.in/api"

# Get groundwater prospects
curl "https://bhuvan-app1.nrsc.gov.in/api/groundwater/prospects?state=PUNJAB&format=json"

# Get aquifer mapping data
curl "https://bhuvan-app1.nrsc.gov.in/api/aquifer/mapping?district=AMRITSAR&format=json"
```

### 4. **State Government Portals**

#### **Punjab State Water Resources**
```bash
# Punjab Water Resources Department
curl "http://www.pbwater.gov.in/api/groundwater/realtime"
curl "http://www.pbwater.gov.in/api/monitoring/wells"
```

#### **Haryana Water Resources**
```bash
# Haryana Irrigation Department
curl "http://www.haryanawater.gov.in/api/groundwater/status"
```

## 🔧 Implementation Steps for Ram

### **Step 1: Set Up Data Fetching Scripts**

Create `scripts/fetch_real_data.py`:
```python
#!/usr/bin/env python3
import requests
import pandas as pd
import psycopg2
from datetime import datetime
import json
import time

class GroundwaterDataFetcher:
    def __init__(self):
        self.cgwb_base = "http://cgwb.gov.in/webgis/services"
        self.nwic_base = "http://www.india-wris.nrsc.gov.in/api"
        self.db_config = {
            'host': 'localhost',
            'database': 'ingres_groundwater',
            'user': 'postgres',
            'password': 'your_password'
        }
    
    def fetch_cgwb_data(self):
        """Fetch data from CGWB API"""
        try:
            # Get state-wise assessment data
            url = f"{self.cgwb_base}/groundwater/MapServer/0/query"
            params = {
                'where': '1=1',
                'outFields': '*',
                'f': 'json'
            }
            
            response = requests.get(url, params=params, timeout=30)
            if response.status_code == 200:
                data = response.json()
                return self.parse_cgwb_response(data)
            else:
                print(f"CGWB API Error: {response.status_code}")
                return None
                
        except Exception as e:
            print(f"Error fetching CGWB data: {e}")
            return None
    
    def fetch_nwic_data(self):
        """Fetch data from NWIC API"""
        try:
            url = f"{self.nwic_base}/groundwater/assessment/latest"
            response = requests.get(url, timeout=30)
            
            if response.status_code == 200:
                return response.json()
            else:
                print(f"NWIC API Error: {response.status_code}")
                return None
                
        except Exception as e:
            print(f"Error fetching NWIC data: {e}")
            return None
    
    def parse_cgwb_response(self, data):
        """Parse CGWB JSON response to our format"""
        parsed_data = []
        
        if 'features' in data:
            for feature in data['features']:
                attrs = feature.get('attributes', {})
                
                record = {
                    'state': attrs.get('STATE_NAME', ''),
                    'district': attrs.get('DISTRICT_NAME', ''),
                    'block': attrs.get('BLOCK_NAME', ''),
                    'category': attrs.get('GW_CATEGORY', ''),
                    'annual_recharge': float(attrs.get('ANNUAL_RECHARGE', 0)),
                    'extractable_resource': float(attrs.get('NET_AVAILABILITY', 0)),
                    'annual_extraction': float(attrs.get('ANNUAL_EXTRACTION', 0)),
                    'assessment_year': int(attrs.get('ASSESSMENT_YEAR', 2023)),
                    'stage_of_extraction': float(attrs.get('STAGE_EXTRACTION', 0)),
                    'last_updated': datetime.now()
                }
                parsed_data.append(record)
        
        return parsed_data
    
    def save_to_database(self, data):
        """Save parsed data to PostgreSQL"""
        try:
            conn = psycopg2.connect(**self.db_config)
            cursor = conn.cursor()
            
            # Create table if not exists
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS groundwater_data (
                    id SERIAL PRIMARY KEY,
                    state VARCHAR(50),
                    district VARCHAR(50),
                    block VARCHAR(50),
                    category VARCHAR(20),
                    annual_recharge FLOAT,
                    extractable_resource FLOAT,
                    annual_extraction FLOAT,
                    assessment_year INTEGER,
                    stage_of_extraction FLOAT,
                    last_updated TIMESTAMP,
                    UNIQUE(state, district, block, assessment_year)
                );
            """)
            
            # Insert data
            for record in data:
                cursor.execute("""
                    INSERT INTO groundwater_data 
                    (state, district, block, category, annual_recharge, 
                     extractable_resource, annual_extraction, assessment_year, 
                     stage_of_extraction, last_updated)
                    VALUES (%(state)s, %(district)s, %(block)s, %(category)s, 
                           %(annual_recharge)s, %(extractable_resource)s, 
                           %(annual_extraction)s, %(assessment_year)s, 
                           %(stage_of_extraction)s, %(last_updated)s)
                    ON CONFLICT (state, district, block, assessment_year) 
                    DO UPDATE SET
                        category = EXCLUDED.category,
                        annual_recharge = EXCLUDED.annual_recharge,
                        extractable_resource = EXCLUDED.extractable_resource,
                        annual_extraction = EXCLUDED.annual_extraction,
                        stage_of_extraction = EXCLUDED.stage_of_extraction,
                        last_updated = EXCLUDED.last_updated;
                """, record)
            
            conn.commit()
            print(f"Successfully saved {len(data)} records to database")
            
        except Exception as e:
            print(f"Database error: {e}")
        finally:
            if conn:
                conn.close()
    
    def run_data_sync(self):
        """Main function to sync all data"""
        print("Starting groundwater data synchronization...")
        
        # Fetch from CGWB
        cgwb_data = self.fetch_cgwb_data()
        if cgwb_data:
            self.save_to_database(cgwb_data)
        
        # Fetch from NWIC
        nwic_data = self.fetch_nwic_data()
        if nwic_data:
            # Parse and save NWIC data (implement parsing logic)
            pass
        
        print("Data synchronization completed!")

if __name__ == "__main__":
    fetcher = GroundwaterDataFetcher()
    fetcher.run_data_sync()
```

### **Step 2: Alternative Data Sources (If APIs Don't Work)**

#### **Manual Download Scripts**
```bash
#!/bin/bash
# scripts/download_cgwb_reports.sh

# Create data directory
mkdir -p data/raw_downloads

# Download latest CGWB assessment reports
echo "Downloading CGWB Assessment Reports..."

# 2023 Assessment Report
wget -O "data/raw_downloads/cgwb_assessment_2023.pdf" \
  "http://cgwb.gov.in/documents/Dynamic-Ground-Water-Resources-India-2023.pdf"

# State-wise groundwater assessment
wget -O "data/raw_downloads/state_wise_assessment.xlsx" \
  "http://cgwb.gov.in/documents/State-wise-Dynamic-Groundwater-Resources-2023.xlsx"

# Water level monitoring data
wget -O "data/raw_downloads/water_level_monitoring.csv" \
  "http://cgwb.gov.in/monitoring/National-Water-Level-Monitoring-2023.csv"

# Aquifer mapping data
wget -O "data/raw_downloads/aquifer_mapping.zip" \
  "http://cgwb.gov.in/aquifer/National-Aquifer-Mapping-Data.zip"

echo "Downloads completed!"
```

#### **Excel/CSV Processing Script**
```python
#!/usr/bin/env python3
# scripts/process_excel_data.py

import pandas as pd
import numpy as np
from datetime import datetime
import psycopg2

def process_cgwb_excel():
    """Process downloaded CGWB Excel files"""
    
    # Read the main assessment file
    df = pd.read_excel('data/raw_downloads/state_wise_assessment.xlsx', 
                       sheet_name='Assessment_Data')
    
    # Clean and standardize column names
    df.columns = df.columns.str.strip().str.lower().str.replace(' ', '_')
    
    # Map columns to our database schema
    column_mapping = {
        'state_name': 'state',
        'district_name': 'district', 
        'block_name': 'block',
        'groundwater_category': 'category',
        'annual_groundwater_recharge': 'annual_recharge',
        'net_annual_groundwater_availability': 'extractable_resource',
        'annual_groundwater_extraction': 'annual_extraction',
        'assessment_year': 'assessment_year'
    }
    
    # Rename columns
    df = df.rename(columns=column_mapping)
    
    # Calculate stage of extraction
    df['stage_of_extraction'] = (df['annual_extraction'] / df['extractable_resource']) * 100
    
    # Add timestamp
    df['last_updated'] = datetime.now()
    
    # Clean data
    df = df.dropna(subset=['state', 'district'])
    df = df[df['state'] != '']
    
    return df

def save_processed_data(df):
    """Save processed data to PostgreSQL"""
    
    db_config = {
        'host': 'localhost',
        'database': 'ingres_groundwater',
        'user': 'postgres', 
        'password': 'your_password'
    }
    
    try:
        conn = psycopg2.connect(**db_config)
        
        # Use pandas to_sql for bulk insert
        df.to_sql('groundwater_data', conn, if_exists='append', 
                  index=False, method='multi')
        
        print(f"Successfully inserted {len(df)} records")
        
    except Exception as e:
        print(f"Error saving data: {e}")
    finally:
        if conn:
            conn.close()

if __name__ == "__main__":
    df = process_cgwb_excel()
    save_processed_data(df)
```

### **Step 3: Real-Time Data Update System**

#### **Automated Data Sync (Cron Job)**
```bash
# Add to crontab: crontab -e

# Sync data every 6 hours
0 */6 * * * /usr/bin/python3 /path/to/scripts/fetch_real_data.py

# Daily backup
0 2 * * * /path/to/scripts/backup_database.sh

# Weekly full sync
0 3 * * 0 /path/to/scripts/full_data_sync.sh
```

#### **Real-time Monitoring Setup**
```python
# scripts/realtime_monitor.py

import time
import requests
from datetime import datetime
import psycopg2

class RealtimeMonitor:
    def __init__(self):
        self.monitoring_wells = [
            {'state': 'Punjab', 'district': 'Amritsar', 'well_id': 'PB_AMR_001'},
            {'state': 'Haryana', 'district': 'Karnal', 'well_id': 'HR_KNL_001'},
            # Add more monitoring wells
        ]
    
    def fetch_well_data(self, well_id):
        """Fetch real-time data from monitoring wells"""
        try:
            url = f"http://cgwb.gov.in/api/monitoring/wells/{well_id}/latest"
            response = requests.get(url, timeout=10)
            
            if response.status_code == 200:
                return response.json()
            return None
            
        except Exception as e:
            print(f"Error fetching well data: {e}")
            return None
    
    def update_realtime_data(self):
        """Update database with real-time monitoring data"""
        for well in self.monitoring_wells:
            data = self.fetch_well_data(well['well_id'])
            if data:
                # Update database with latest readings
                self.save_monitoring_data(well, data)
    
    def run_continuous_monitoring(self):
        """Run continuous monitoring loop"""
        while True:
            print(f"Updating real-time data at {datetime.now()}")
            self.update_realtime_data()
            time.sleep(3600)  # Update every hour

if __name__ == "__main__":
    monitor = RealtimeMonitor()
    monitor.run_continuous_monitoring()
```

## 📊 Database Schema for Real Data

### **PostgreSQL Table Structure**
```sql
-- Create main groundwater data table
CREATE TABLE groundwater_data (
    id SERIAL PRIMARY KEY,
    state VARCHAR(50) NOT NULL,
    district VARCHAR(50) NOT NULL,
    block VARCHAR(50),
    category VARCHAR(20) CHECK (category IN ('Safe', 'Semi-Critical', 'Critical', 'Over-Exploited')),
    annual_recharge FLOAT,
    extractable_resource FLOAT,
    annual_extraction FLOAT,
    assessment_year INTEGER,
    stage_of_extraction FLOAT,
    latitude FLOAT,
    longitude FLOAT,
    last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    data_source VARCHAR(50) DEFAULT 'CGWB',
    UNIQUE(state, district, block, assessment_year)
);

-- Create monitoring wells table for real-time data
CREATE TABLE monitoring_wells (
    id SERIAL PRIMARY KEY,
    well_id VARCHAR(50) UNIQUE,
    state VARCHAR(50),
    district VARCHAR(50),
    latitude FLOAT,
    longitude FLOAT,
    depth_meters FLOAT,
    installation_date DATE,
    status VARCHAR(20) DEFAULT 'Active'
);

-- Create real-time readings table
CREATE TABLE well_readings (
    id SERIAL PRIMARY KEY,
    well_id VARCHAR(50) REFERENCES monitoring_wells(well_id),
    water_level_meters FLOAT,
    reading_timestamp TIMESTAMP,
    temperature FLOAT,
    ph_level FLOAT,
    data_quality VARCHAR(20) DEFAULT 'Good'
);

-- Create indexes for performance
CREATE INDEX idx_groundwater_state_district ON groundwater_data(state, district);
CREATE INDEX idx_groundwater_category ON groundwater_data(category);
CREATE INDEX idx_groundwater_year ON groundwater_data(assessment_year);
CREATE INDEX idx_well_readings_timestamp ON well_readings(reading_timestamp);
```

## 🚀 Quick Start Commands for Ram

### **1. Set Up Environment**
```bash
# Install required packages
pip install requests pandas psycopg2-binary openpyxl

# Set up PostgreSQL database
sudo -u postgres createdb ingres_groundwater
sudo -u postgres psql ingres_groundwater < schema.sql
```

### **2. Run Initial Data Sync**
```bash
# Download and process data
python3 scripts/fetch_real_data.py

# If APIs don't work, use manual download
bash scripts/download_cgwb_reports.sh
python3 scripts/process_excel_data.py
```

### **3. Set Up Automated Updates**
```bash
# Add cron jobs for automatic updates
crontab -e

# Add these lines:
0 */6 * * * /usr/bin/python3 /path/to/scripts/fetch_real_data.py
0 2 * * * /path/to/scripts/backup_database.sh
```

## 📞 Contact Information for Data Access

### **Official Contacts**
- **CGWB Headquarters**: +91-11-24364473
- **NWIC Help Desk**: +91-40-23884200
- **Data Request Email**: data.cgwb@gov.in

### **State Water Departments**
- **Punjab**: +91-172-2740362
- **Haryana**: +91-172-2560349
- **Gujarat**: +91-79-23251501

## ⚠️ Important Notes

1. **API Rate Limits**: Government APIs may have rate limits (usually 100 requests/hour)
2. **Data Lag**: Official data is typically 3-6 months behind real-time
3. **Authentication**: Some APIs may require registration/API keys
4. **Backup Plan**: Always have manual download scripts as fallback
5. **Data Validation**: Implement data quality checks before database insertion

## 🎯 Success Metrics

- **Data Coverage**: Target 28 states, 700+ districts
- **Update Frequency**: Every 6 hours for available data
- **Data Quality**: >95% valid records
- **Response Time**: <2 seconds for database queries

**This guide gives Ram everything needed to set up real-time groundwater data integration! 🚀**
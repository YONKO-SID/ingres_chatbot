#!/usr/bin/env python3
"""
INGRES ChatBot - Real-time CGWB Data Fetcher
Fetches groundwater data from official government sources
"""

import requests
import pandas as pd
import psycopg2
from psycopg2.extras import RealDictCursor
import json
import time
from datetime import datetime, timedelta
import logging
import os
import sys
from typing import List, Dict, Optional

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('logs/data_sync.log'),
        logging.StreamHandler(sys.stdout)
    ]
)
logger = logging.getLogger(__name__)

class CGWBDataFetcher:
    """Fetches and processes groundwater data from CGWB and other sources"""
    
    def __init__(self):
        self.cgwb_endpoints = {
            'assessment': 'http://cgwb.gov.in/webgis/services/groundwater/MapServer/0/query',
            'monitoring': 'http://cgwb.gov.in/webgis/services/monitoring/MapServer/0/query',
            'wells': 'http://cgwb.gov.in/api/wells/active'
        }
        
        self.nwic_endpoints = {
            'groundwater': 'http://www.india-wris.nrsc.gov.in/api/groundwater',
            'assessment': 'http://www.india-wris.nrsc.gov.in/api/assessment/latest'
        }
        
        # Database configuration
        self.db_config = {
            'host': os.getenv('DB_HOST', 'localhost'),
            'database': os.getenv('DB_NAME', 'ingres_groundwater'),
            'user': os.getenv('DB_USER', 'postgres'),
            'password': os.getenv('DB_PASSWORD', 'password'),
            'port': os.getenv('DB_PORT', '5432')
        }
        
        # Request configuration
        self.session = requests.Session()
        self.session.headers.update({
            'User-Agent': 'INGRES-ChatBot/1.0 (Educational Research)',
            'Accept': 'application/json'
        })
        
        self.timeout = 30
        self.retry_count = 3
        self.retry_delay = 5
    
    def fetch_with_retry(self, url: str, params: Dict = None) -> Optional[Dict]:
        """Fetch data with retry logic"""
        for attempt in range(self.retry_count):
            try:
                logger.info(f"Fetching data from {url} (attempt {attempt + 1})")
                response = self.session.get(url, params=params, timeout=self.timeout)
                
                if response.status_code == 200:
                    return response.json()
                elif response.status_code == 429:  # Rate limited
                    logger.warning(f"Rate limited, waiting {self.retry_delay * (attempt + 1)} seconds")
                    time.sleep(self.retry_delay * (attempt + 1))
                else:
                    logger.error(f"HTTP {response.status_code}: {response.text}")
                    
            except requests.exceptions.Timeout:
                logger.warning(f"Timeout on attempt {attempt + 1}")
            except requests.exceptions.RequestException as e:
                logger.error(f"Request error: {e}")
            
            if attempt < self.retry_count - 1:
                time.sleep(self.retry_delay)
        
        return None
    
    def fetch_cgwb_assessment_data(self) -> List[Dict]:
        """Fetch groundwater assessment data from CGWB"""
        logger.info("Fetching CGWB assessment data...")
        
        params = {
            'where': '1=1',
            'outFields': '*',
            'f': 'json',
            'returnGeometry': 'false'
        }
        
        data = self.fetch_with_retry(self.cgwb_endpoints['assessment'], params)
        
        if not data or 'features' not in data:
            logger.error("No valid data received from CGWB assessment API")
            return []
        
        records = []
        for feature in data['features']:
            attrs = feature.get('attributes', {})
            
            # Map CGWB fields to our schema
            record = {
                'state': self.clean_string(attrs.get('STATE_NAME', attrs.get('State', ''))),
                'district': self.clean_string(attrs.get('DISTRICT_NAME', attrs.get('District', ''))),
                'block': self.clean_string(attrs.get('BLOCK_NAME', attrs.get('Block', ''))),
                'category': self.normalize_category(attrs.get('GW_CATEGORY', attrs.get('Category', ''))),
                'annual_recharge': self.safe_float(attrs.get('ANNUAL_RECHARGE', attrs.get('Recharge', 0))),
                'extractable_resource': self.safe_float(attrs.get('NET_AVAILABILITY', attrs.get('Availability', 0))),
                'annual_extraction': self.safe_float(attrs.get('ANNUAL_EXTRACTION', attrs.get('Extraction', 0))),
                'assessment_year': self.safe_int(attrs.get('ASSESSMENT_YEAR', attrs.get('Year', 2023))),
                'latitude': self.safe_float(attrs.get('LATITUDE', attrs.get('Lat', None))),
                'longitude': self.safe_float(attrs.get('LONGITUDE', attrs.get('Lon', None))),
                'data_source': 'CGWB_API'
            }
            
            # Calculate stage of extraction
            if record['extractable_resource'] and record['extractable_resource'] > 0:
                record['stage_of_extraction'] = (record['annual_extraction'] / record['extractable_resource']) * 100
            else:
                record['stage_of_extraction'] = 0
            
            # Validate required fields
            if record['state'] and record['district']:
                records.append(record)
        
        logger.info(f"Processed {len(records)} records from CGWB assessment data")
        return records
    
    def fetch_nwic_data(self) -> List[Dict]:
        """Fetch data from NWIC (India-WRIS)"""
        logger.info("Fetching NWIC groundwater data...")
        
        # Try different NWIC endpoints
        endpoints_to_try = [
            'http://www.india-wris.nrsc.gov.in/api/groundwater/assessment',
            'http://india-wris.nrsc.gov.in/api/data/groundwater',
            'https://indiawris.gov.in/api/groundwater'
        ]
        
        for endpoint in endpoints_to_try:
            data = self.fetch_with_retry(endpoint)
            if data:
                return self.parse_nwic_data(data)
        
        logger.warning("Could not fetch data from any NWIC endpoint")
        return []
    
    def parse_nwic_data(self, data: Dict) -> List[Dict]:
        """Parse NWIC data format"""
        records = []
        
        # NWIC data structure varies, handle different formats
        if isinstance(data, dict):
            if 'data' in data:
                data = data['data']
            elif 'results' in data:
                data = data['results']
        
        if isinstance(data, list):
            for item in data:
                record = {
                    'state': self.clean_string(item.get('state', item.get('State', ''))),
                    'district': self.clean_string(item.get('district', item.get('District', ''))),
                    'block': self.clean_string(item.get('block', item.get('Block', ''))),
                    'category': self.normalize_category(item.get('category', item.get('Category', ''))),
                    'annual_recharge': self.safe_float(item.get('recharge', 0)),
                    'extractable_resource': self.safe_float(item.get('availability', 0)),
                    'annual_extraction': self.safe_float(item.get('extraction', 0)),
                    'assessment_year': self.safe_int(item.get('year', 2023)),
                    'data_source': 'NWIC_API'
                }
                
                if record['state'] and record['district']:
                    records.append(record)
        
        logger.info(f"Processed {len(records)} records from NWIC data")
        return records
    
    def fetch_manual_sources(self) -> List[Dict]:
        """Fetch from manual/backup data sources"""
        logger.info("Attempting to fetch from manual sources...")
        
        # Try to download latest CGWB reports
        manual_urls = [
            'http://cgwb.gov.in/documents/Dynamic-Ground-Water-Resources-India-2023.csv',
            'http://cgwb.gov.in/data/groundwater-assessment-2023.json',
            'https://data.gov.in/resource/dynamic-ground-water-resources-india-march-2020'
        ]
        
        for url in manual_urls:
            try:
                response = self.session.get(url, timeout=self.timeout)
                if response.status_code == 200:
                    if url.endswith('.csv'):
                        return self.parse_csv_data(response.text)
                    elif url.endswith('.json'):
                        return self.parse_json_data(response.json())
            except Exception as e:
                logger.warning(f"Failed to fetch from {url}: {e}")
        
        return []
    
    def parse_csv_data(self, csv_text: str) -> List[Dict]:
        """Parse CSV data from manual sources"""
        try:
            import io
            df = pd.read_csv(io.StringIO(csv_text))
            
            # Standardize column names
            df.columns = df.columns.str.strip().str.lower().str.replace(' ', '_')
            
            records = []
            for _, row in df.iterrows():
                record = {
                    'state': self.clean_string(row.get('state', '')),
                    'district': self.clean_string(row.get('district', '')),
                    'block': self.clean_string(row.get('block', '')),
                    'category': self.normalize_category(row.get('category', '')),
                    'annual_recharge': self.safe_float(row.get('recharge', 0)),
                    'extractable_resource': self.safe_float(row.get('availability', 0)),
                    'annual_extraction': self.safe_float(row.get('extraction', 0)),
                    'assessment_year': self.safe_int(row.get('year', 2023)),
                    'data_source': 'Manual_CSV'
                }
                
                if record['state'] and record['district']:
                    records.append(record)
            
            return records
            
        except Exception as e:
            logger.error(f"Error parsing CSV data: {e}")
            return []
    
    def save_to_database(self, records: List[Dict]) -> Dict[str, int]:
        """Save records to PostgreSQL database"""
        if not records:
            return {'inserted': 0, 'updated': 0, 'errors': 0}
        
        stats = {'inserted': 0, 'updated': 0, 'errors': 0}
        
        try:
            conn = psycopg2.connect(**self.db_config)
            cursor = conn.cursor()
            
            for record in records:
                try:
                    # Use UPSERT (INSERT ... ON CONFLICT)
                    cursor.execute("""
                        INSERT INTO groundwater_data 
                        (state, district, block, category, annual_recharge, 
                         extractable_resource, annual_extraction, assessment_year, 
                         stage_of_extraction, latitude, longitude, data_source, last_updated)
                        VALUES (%(state)s, %(district)s, %(block)s, %(category)s, 
                               %(annual_recharge)s, %(extractable_resource)s, 
                               %(annual_extraction)s, %(assessment_year)s, 
                               %(stage_of_extraction)s, %(latitude)s, %(longitude)s,
                               %(data_source)s, CURRENT_TIMESTAMP)
                        ON CONFLICT (state, district, block, assessment_year) 
                        DO UPDATE SET
                            category = EXCLUDED.category,
                            annual_recharge = EXCLUDED.annual_recharge,
                            extractable_resource = EXCLUDED.extractable_resource,
                            annual_extraction = EXCLUDED.annual_extraction,
                            stage_of_extraction = EXCLUDED.stage_of_extraction,
                            latitude = EXCLUDED.latitude,
                            longitude = EXCLUDED.longitude,
                            data_source = EXCLUDED.data_source,
                            last_updated = CURRENT_TIMESTAMP
                        RETURNING (xmax = 0) AS inserted;
                    """, record)
                    
                    result = cursor.fetchone()
                    if result and result[0]:
                        stats['inserted'] += 1
                    else:
                        stats['updated'] += 1
                        
                except Exception as e:
                    logger.error(f"Error inserting record {record}: {e}")
                    stats['errors'] += 1
            
            conn.commit()
            logger.info(f"Database operation completed: {stats}")
            
        except Exception as e:
            logger.error(f"Database connection error: {e}")
            stats['errors'] = len(records)
        finally:
            if 'conn' in locals():
                conn.close()
        
        return stats
    
    def log_sync_operation(self, sync_type: str, stats: Dict[str, int], 
                          start_time: datetime, error_message: str = None):
        """Log synchronization operation"""
        try:
            conn = psycopg2.connect(**self.db_config)
            cursor = conn.cursor()
            
            status = 'Success' if stats['errors'] == 0 else ('Partial' if stats['inserted'] > 0 or stats['updated'] > 0 else 'Failed')
            
            cursor.execute("""
                INSERT INTO data_sync_logs 
                (sync_type, records_processed, records_inserted, records_updated, 
                 errors_count, sync_started_at, sync_completed_at, status, error_message)
                VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
            """, (
                sync_type,
                stats['inserted'] + stats['updated'] + stats['errors'],
                stats['inserted'],
                stats['updated'],
                stats['errors'],
                start_time,
                datetime.now(),
                status,
                error_message
            ))
            
            conn.commit()
            
        except Exception as e:
            logger.error(f"Error logging sync operation: {e}")
        finally:
            if 'conn' in locals():
                conn.close()
    
    def run_full_sync(self):
        """Run complete data synchronization"""
        logger.info("Starting full groundwater data synchronization...")
        start_time = datetime.now()
        
        all_records = []
        error_messages = []
        
        # Try CGWB API first
        try:
            cgwb_records = self.fetch_cgwb_assessment_data()
            all_records.extend(cgwb_records)
            logger.info(f"Fetched {len(cgwb_records)} records from CGWB")
        except Exception as e:
            error_msg = f"CGWB API error: {e}"
            logger.error(error_msg)
            error_messages.append(error_msg)
        
        # Try NWIC API
        try:
            nwic_records = self.fetch_nwic_data()
            all_records.extend(nwic_records)
            logger.info(f"Fetched {len(nwic_records)} records from NWIC")
        except Exception as e:
            error_msg = f"NWIC API error: {e}"
            logger.error(error_msg)
            error_messages.append(error_msg)
        
        # Try manual sources if APIs failed
        if not all_records:
            try:
                manual_records = self.fetch_manual_sources()
                all_records.extend(manual_records)
                logger.info(f"Fetched {len(manual_records)} records from manual sources")
            except Exception as e:
                error_msg = f"Manual sources error: {e}"
                logger.error(error_msg)
                error_messages.append(error_msg)
        
        # Save to database
        if all_records:
            stats = self.save_to_database(all_records)
            logger.info(f"Sync completed: {stats}")
        else:
            stats = {'inserted': 0, 'updated': 0, 'errors': 0}
            logger.error("No data fetched from any source")
        
        # Log the operation
        self.log_sync_operation(
            'FULL_SYNC', 
            stats, 
            start_time, 
            '; '.join(error_messages) if error_messages else None
        )
        
        return stats
    
    # Utility methods
    def clean_string(self, value) -> str:
        """Clean and normalize string values"""
        if not value or pd.isna(value):
            return ''
        return str(value).strip().title()
    
    def safe_float(self, value) -> float:
        """Safely convert to float"""
        try:
            if pd.isna(value) or value == '':
                return 0.0
            return float(value)
        except (ValueError, TypeError):
            return 0.0
    
    def safe_int(self, value) -> int:
        """Safely convert to int"""
        try:
            if pd.isna(value) or value == '':
                return 2023
            return int(float(value))
        except (ValueError, TypeError):
            return 2023
    
    def normalize_category(self, category: str) -> str:
        """Normalize groundwater category names"""
        if not category:
            return 'Unknown'
        
        category = category.strip().lower()
        
        if 'over' in category and 'exploit' in category:
            return 'Over-Exploited'
        elif 'critical' in category:
            return 'Critical'
        elif 'semi' in category and 'critical' in category:
            return 'Semi-Critical'
        elif 'safe' in category:
            return 'Safe'
        else:
            return 'Unknown'

def main():
    """Main execution function"""
    # Create logs directory if it doesn't exist
    os.makedirs('logs', exist_ok=True)
    
    # Initialize fetcher
    fetcher = CGWBDataFetcher()
    
    # Run synchronization
    try:
        stats = fetcher.run_full_sync()
        
        if stats['inserted'] > 0 or stats['updated'] > 0:
            print(f"✅ Sync successful: {stats['inserted']} inserted, {stats['updated']} updated")
            return 0
        else:
            print(f"❌ Sync failed: {stats['errors']} errors")
            return 1
            
    except Exception as e:
        logger.error(f"Fatal error during sync: {e}")
        print(f"❌ Fatal error: {e}")
        return 1

if __name__ == "__main__":
    exit(main())
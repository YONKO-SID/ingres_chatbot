-- INGRES ChatBot - PostgreSQL Database Setup
-- Run this file to create the complete database schema

-- Create database (run as postgres user)
-- CREATE DATABASE ingres_groundwater;
-- \c ingres_groundwater;

-- Enable extensions
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "postgis";

-- Main groundwater assessment data table
CREATE TABLE IF NOT EXISTS groundwater_data (
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
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(state, district, block, assessment_year)
);

-- Monitoring wells for real-time data
CREATE TABLE IF NOT EXISTS monitoring_wells (
    id SERIAL PRIMARY KEY,
    well_id VARCHAR(50) UNIQUE NOT NULL,
    state VARCHAR(50) NOT NULL,
    district VARCHAR(50) NOT NULL,
    block VARCHAR(50),
    latitude FLOAT,
    longitude FLOAT,
    depth_meters FLOAT,
    installation_date DATE,
    status VARCHAR(20) DEFAULT 'Active',
    well_type VARCHAR(30) DEFAULT 'Observation',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Real-time water level readings
CREATE TABLE IF NOT EXISTS well_readings (
    id SERIAL PRIMARY KEY,
    well_id VARCHAR(50) REFERENCES monitoring_wells(well_id),
    water_level_meters FLOAT,
    reading_timestamp TIMESTAMP NOT NULL,
    temperature FLOAT,
    ph_level FLOAT,
    electrical_conductivity FLOAT,
    data_quality VARCHAR(20) DEFAULT 'Good',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Historical trends table
CREATE TABLE IF NOT EXISTS historical_trends (
    id SERIAL PRIMARY KEY,
    state VARCHAR(50) NOT NULL,
    district VARCHAR(50) NOT NULL,
    block VARCHAR(50),
    year INTEGER NOT NULL,
    avg_water_level FLOAT,
    trend_direction VARCHAR(20), -- 'Rising', 'Declining', 'Stable'
    trend_rate FLOAT, -- meters per year
    calculated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(state, district, block, year)
);

-- Data sync logs
CREATE TABLE IF NOT EXISTS data_sync_logs (
    id SERIAL PRIMARY KEY,
    sync_type VARCHAR(50) NOT NULL, -- 'CGWB_API', 'NWIC_API', 'Manual_Upload'
    records_processed INTEGER,
    records_inserted INTEGER,
    records_updated INTEGER,
    errors_count INTEGER,
    sync_started_at TIMESTAMP,
    sync_completed_at TIMESTAMP,
    status VARCHAR(20) DEFAULT 'Success', -- 'Success', 'Failed', 'Partial'
    error_message TEXT
);

-- Create indexes for performance
CREATE INDEX IF NOT EXISTS idx_groundwater_state_district ON groundwater_data(state, district);
CREATE INDEX IF NOT EXISTS idx_groundwater_category ON groundwater_data(category);
CREATE INDEX IF NOT EXISTS idx_groundwater_year ON groundwater_data(assessment_year);
CREATE INDEX IF NOT EXISTS idx_groundwater_stage ON groundwater_data(stage_of_extraction);
CREATE INDEX IF NOT EXISTS idx_well_readings_timestamp ON well_readings(reading_timestamp);
CREATE INDEX IF NOT EXISTS idx_well_readings_well_id ON well_readings(well_id);
CREATE INDEX IF NOT EXISTS idx_historical_trends_location ON historical_trends(state, district, year);

-- Create views for common queries
CREATE OR REPLACE VIEW v_latest_assessment AS
SELECT DISTINCT ON (state, district, block) 
    state, district, block, category, annual_recharge, 
    extractable_resource, annual_extraction, stage_of_extraction,
    assessment_year, last_updated
FROM groundwater_data 
ORDER BY state, district, block, assessment_year DESC;

CREATE OR REPLACE VIEW v_critical_areas AS
SELECT state, district, block, category, stage_of_extraction, assessment_year
FROM groundwater_data 
WHERE category IN ('Critical', 'Over-Exploited')
ORDER BY stage_of_extraction DESC;

CREATE OR REPLACE VIEW v_state_summary AS
SELECT 
    state,
    COUNT(*) as total_blocks,
    COUNT(CASE WHEN category = 'Safe' THEN 1 END) as safe_blocks,
    COUNT(CASE WHEN category = 'Semi-Critical' THEN 1 END) as semi_critical_blocks,
    COUNT(CASE WHEN category = 'Critical' THEN 1 END) as critical_blocks,
    COUNT(CASE WHEN category = 'Over-Exploited' THEN 1 END) as over_exploited_blocks,
    AVG(stage_of_extraction) as avg_extraction_stage,
    MAX(assessment_year) as latest_assessment_year
FROM v_latest_assessment 
GROUP BY state
ORDER BY over_exploited_blocks DESC, critical_blocks DESC;

-- Insert sample data for testing (remove in production)
INSERT INTO groundwater_data (state, district, block, category, annual_recharge, extractable_resource, annual_extraction, assessment_year, stage_of_extraction) VALUES
('Punjab', 'Amritsar', 'Ajnala', 'Over-Exploited', 156.8, 89.2, 178.5, 2023, 200.1),
('Punjab', 'Ludhiana', 'Ludhiana-I', 'Over-Exploited', 198.7, 112.3, 225.4, 2023, 200.7),
('Haryana', 'Karnal', 'Karnal', 'Safe', 78.5, 89.4, 67.8, 2023, 75.8),
('Gujarat', 'Ahmedabad', 'Ahmedabad City', 'Critical', 156.7, 112.4, 167.8, 2023, 149.3),
('Maharashtra', 'Pune', 'Pune City', 'Critical', 145.6, 98.7, 156.8, 2023, 158.9)
ON CONFLICT (state, district, block, assessment_year) DO NOTHING;

-- Create functions for common operations
CREATE OR REPLACE FUNCTION get_state_data(state_name TEXT)
RETURNS TABLE(
    district TEXT,
    block TEXT,
    category TEXT,
    stage_of_extraction FLOAT,
    assessment_year INTEGER
) AS $$
BEGIN
    RETURN QUERY
    SELECT gd.district, gd.block, gd.category, gd.stage_of_extraction, gd.assessment_year
    FROM v_latest_assessment gd
    WHERE gd.state = state_name
    ORDER BY gd.stage_of_extraction DESC;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_critical_areas_count()
RETURNS TABLE(
    state TEXT,
    critical_count BIGINT,
    over_exploited_count BIGINT
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        gd.state,
        COUNT(CASE WHEN gd.category = 'Critical' THEN 1 END) as critical_count,
        COUNT(CASE WHEN gd.category = 'Over-Exploited' THEN 1 END) as over_exploited_count
    FROM v_latest_assessment gd
    WHERE gd.category IN ('Critical', 'Over-Exploited')
    GROUP BY gd.state
    ORDER BY over_exploited_count DESC, critical_count DESC;
END;
$$ LANGUAGE plpgsql;

-- Grant permissions (adjust as needed)
-- GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO ingres_user;
-- GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO ingres_user;

COMMENT ON TABLE groundwater_data IS 'Main table storing CGWB groundwater assessment data';
COMMENT ON TABLE monitoring_wells IS 'Real-time monitoring wells information';
COMMENT ON TABLE well_readings IS 'Time-series data from monitoring wells';
COMMENT ON TABLE historical_trends IS 'Calculated historical trends and patterns';
COMMENT ON TABLE data_sync_logs IS 'Logs of data synchronization operations';

-- Display setup completion message
DO $$
BEGIN
    RAISE NOTICE 'INGRES ChatBot database setup completed successfully!';
    RAISE NOTICE 'Tables created: groundwater_data, monitoring_wells, well_readings, historical_trends, data_sync_logs';
    RAISE NOTICE 'Views created: v_latest_assessment, v_critical_areas, v_state_summary';
    RAISE NOTICE 'Functions created: get_state_data(), get_critical_areas_count()';
END $$;
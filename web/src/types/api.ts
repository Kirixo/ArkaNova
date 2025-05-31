export interface ApiResponse<T> {
  data?: T;
  error?: string;
}

export interface SensorType {
  id: number;
  name: string;
}

export interface Sensor {
  id: number;
  type: SensorType;
  solar_panel_id: number;
  status?: 'active' | 'inactive' | 'error' | 'unknown';
  last_updated?: number;
}

export interface SensorListResponse {
  sensors: Sensor[];
  total_count: number;
}

export interface Measurement {
  data: number;
  id: number;
  recorded_at: number;
}

export interface MeasurementListResponse {
  measurements: Measurement[];
}

export interface LatestMeasurement {
  data: number;
  id: number;
  recorded_at: number;
}

export interface SolarPanel {
  id: number;
  user_id: number;
  location: string;
  created_at: number;
  updated_at?: number;
}

export interface SolarPanelListResponse {
  panels: SolarPanel[];
  total_count: number;
}

export interface PanelMeasurements {
  [key: string]: { // type name as key (e.g., "temperature")
    average: number;
    timestamp: number;
  };
} 
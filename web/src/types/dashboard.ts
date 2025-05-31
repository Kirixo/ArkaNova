export type TimeRange = 'hourly' | 'daily' | 'weekly';

export interface DashboardFilters {
  panelId: number | null;
  sensorId: number | null;
  timeRange: TimeRange;
}

interface ChartDataset {
  label: string;
  data: number[];
  borderColor?: string;
  backgroundColor?: string;
  tension?: number;
}

export interface ChartData {
  labels: string[];
  datasets: ChartDataset[];
}

export interface Summary {
  averageTemperature: number | null;
  totalEnergy: number | null;
} 
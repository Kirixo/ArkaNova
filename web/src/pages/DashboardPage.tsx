import React, { useEffect, useState, useCallback, useMemo } from 'react';
import { useTranslation } from 'react-i18next';
import {
  Box,
  Card,
  CardContent,
  Typography,
  Select,
  MenuItem,
  FormControl,
  InputLabel,
  Alert,
  Snackbar,
  Skeleton,
} from '@mui/material';
import { Line } from 'react-chartjs-2';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from 'chart.js';
import { format } from 'date-fns';
import { enUS, uk } from 'date-fns/locale';
import Navigation from '../components/Navigation';
import { API_CONFIG } from '../config/api';
import type { SolarPanel, Sensor, SensorListResponse, MeasurementListResponse } from '../types/api';
import type { TimeRange, DashboardFilters, ChartData } from '../types/dashboard';

// Register ChartJS components
ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

const DashboardPage: React.FC = () => {
  const { t, i18n } = useTranslation(['dashboard', 'common']);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [panels, setPanels] = useState<SolarPanel[]>([]);
  const [sensors, setSensors] = useState<Sensor[]>([]);
  const [filters, setFilters] = useState<DashboardFilters>({
    panelId: null,
    sensorId: null,
    timeRange: 'hourly',
  });
  const [chartData, setChartData] = useState<ChartData | null>(null);
  const user = JSON.parse(localStorage.getItem('user') || '{}');

  const formatDate = useCallback((timestamp: number) => {
    return format(new Date(timestamp), 'PPpp', {
      locale: i18n.language === 'uk-UA' ? uk : enUS,
    });
  }, [i18n.language]);

  // Add language initialization
  useEffect(() => {
    // Set default language if not set
    if (!i18n.language) {
      i18n.changeLanguage('en');
    }
  }, [i18n]);

  // Load panels
  useEffect(() => {
    const loadPanels = async () => {
      try {
        const response = await fetch(
          `${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.SOLAR_PANELS}?user_id=${user.id}&page=1&limit=25`,
          {
            headers: {
              'Content-Type': 'application/json',
              'ngrok-skip-browser-warning': 'true',
            },
            credentials: 'include',
          }
        );

        if (!response.ok) {
          setError(t('dashboard:errors.loadFailed'));
          return;
        }

        const data = await response.json();
        setPanels(data.panels || []);

        // Set first panel as default if available
        if (data.panels?.length > 0) {
          setFilters(prev => ({ ...prev, panelId: data.panels[0].id }));
        }
      } catch (err) {
        console.error('Failed to load panels:', err);
        setError(t('dashboard:errors.loadFailed'));
      }
    };

    loadPanels();
  }, [t, user.id]);

  // Load sensors when panel is selected
  useEffect(() => {
    const loadSensors = async () => {
      if (!filters.panelId) return;

      try {
        const response = await fetch(
          `${API_CONFIG.BASE_URL}/api/sensor/list/solarpanel?panel_id=${filters.panelId}&page=1&limit=25`,
          {
            headers: {
              'Content-Type': 'application/json',
              'ngrok-skip-browser-warning': 'true',
            },
            credentials: 'include',
          }
        );

        if (!response.ok) {
          setError(t('dashboard:errors.loadFailed'));
          return;
        }

        const data: SensorListResponse = await response.json();
        setSensors(data.sensors || []);

        // Set first sensor as default if available
        if (data.sensors?.length > 0) {
          setFilters(prev => ({ ...prev, sensorId: data.sensors[0].id }));
        }
      } catch (err) {
        console.error('Failed to load sensors:', err);
        setError(t('dashboard:errors.loadFailed'));
      }
    };

    loadSensors();
  }, [filters.panelId, t]);

  const loadMeasurements = useCallback(async () => {
    if (!filters.sensorId) return;
    setLoading(true);

    try {
      const now = new Date();
      let startDate = new Date();

      switch (filters.timeRange) {
        case 'hourly':
          startDate.setHours(startDate.getHours() - 24);
          break;
        case 'daily':
          startDate.setDate(startDate.getDate() - 7);
          break;
        case 'weekly':
          startDate.setDate(startDate.getDate() - 30);
          break;
      }

      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/measurement/list/sensor?sensor_id=${filters.sensorId}&start_date=${startDate.toISOString()}&end_date=${now.toISOString()}`,
        {
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (!response.ok) {
        setError(t('dashboard:errors.loadFailed'));
        return;
      }

      const data: MeasurementListResponse = await response.json();
      const measurements = data.measurements || [];

      if (measurements.length === 0) {
        setError(t('dashboard:errors.noData'));
        setChartData(null);
        return;
      }

      // Sort measurements by timestamp in ascending order (left to right)
      const sortedMeasurements = [...measurements].sort((a, b) => a.recorded_at - b.recorded_at);

      // Get the sensor type
      const currentSensor = sensors.find(s => s.id === filters.sensorId);
      const sensorType = currentSensor?.type.name;

      const chartColor = sensorType === 'temperature' ? '#4CAF50' : '#2196F3';

      setChartData({
        labels: sortedMeasurements.map(m => formatDate(m.recorded_at)),
        datasets: [{
          label: t(`dashboard:sensorTypes.${sensorType}`),
          data: sortedMeasurements.map(m => m.data),
          borderColor: chartColor,
          backgroundColor: `${chartColor}20`,
          tension: 0.4,
        }],
      });
    } catch (err) {
      console.error('Failed to load measurements:', err);
      setError(t('dashboard:errors.loadFailed'));
    } finally {
      setLoading(false);
    }
  }, [filters.sensorId, filters.timeRange, formatDate, sensors, t]);

  useEffect(() => {
    loadMeasurements();
  }, [loadMeasurements]);

  const chartOptions = useMemo(() => {
    const currentSensor = sensors.find(s => s.id === filters.sensorId);
    const sensorType = currentSensor?.type.name;

    return {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        tooltip: {
          mode: 'index' as const,
          intersect: false,
        },
        legend: {
          display: false,
        },
      },
      scales: {
        x: {
          grid: {
            display: false,
          },
        },
        y: {
          beginAtZero: true,
          title: {
            display: true,
            text: sensorType ? t(`dashboard:charts.${sensorType}.yAxis`) : '',
          },
        },
      },
    };
  }, [sensors, filters.sensorId, t]);

  return (
    <Box 
      sx={{ 
        display: 'flex',
        minHeight: '100vh',
        backgroundColor: '#F1F1F1',
      }}
    >
      <Navigation isAdmin={user.role === 'admin'} />
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          p: 3,
          marginLeft: `${64}px`, // Navigation width
          display: 'flex',
          flexDirection: 'column',
          alignItems: 'center',
          maxWidth: `calc(100% - ${64}px)`, // Subtract navigation width
          margin: '0 auto',
        }}
      >
        <Box
          sx={{
            width: '100%',
            maxWidth: '1200px',
            margin: '0 auto',
            display: 'flex',
            flexDirection: 'column',
            gap: 4,
          }}
        >
          <Typography 
            variant="h1" 
            sx={{ 
              fontSize: '24px', 
              fontWeight: 700, 
              fontFamily: 'Roboto',
              textAlign: 'center',
            }}
          >
            {t('dashboard:title')}
          </Typography>

          <Box sx={{ display: 'flex', gap: 2, flexWrap: 'wrap' }}>
            <FormControl sx={{ minWidth: 250, flex: 1 }}>
              <InputLabel id="panel-select-label">
                {t('dashboard:filters.panel')}
              </InputLabel>
              <Select
                labelId="panel-select-label"
                value={filters.panelId || ''}
                label={t('dashboard:filters.panel')}
                onChange={(e) => setFilters(prev => ({ ...prev, panelId: e.target.value as number }))}
              >
                {panels.map(panel => (
                  <MenuItem key={panel.id} value={panel.id}>
                    {t('dashboard:filters.panelName', { id: panel.id, location: panel.location })}
                  </MenuItem>
                ))}
              </Select>
            </FormControl>

            <FormControl sx={{ minWidth: 250, flex: 1 }}>
              <InputLabel id="sensor-select-label">
                {t('dashboard:filters.sensor')}
              </InputLabel>
              <Select
                labelId="sensor-select-label"
                value={filters.sensorId || ''}
                label={t('dashboard:filters.sensor')}
                onChange={(e) => setFilters(prev => ({ ...prev, sensorId: e.target.value as number }))}
              >
                {sensors.map(sensor => (
                  <MenuItem key={sensor.id} value={sensor.id}>
                    {t('dashboard:filters.sensorName', { 
                      id: sensor.id, 
                      type: t(`dashboard:sensorTypes.${sensor.type.name}`)
                    })}
                  </MenuItem>
                ))}
              </Select>
            </FormControl>

            <FormControl sx={{ minWidth: 250, flex: 1 }}>
              <InputLabel id="time-range-select-label">
                {t('dashboard:filters.timeRange')}
              </InputLabel>
              <Select
                labelId="time-range-select-label"
                value={filters.timeRange}
                label={t('dashboard:filters.timeRange')}
                onChange={(e) => setFilters(prev => ({ ...prev, timeRange: e.target.value as TimeRange }))}
              >
                <MenuItem value="hourly">{t('dashboard:filters.timeRanges.hourly')}</MenuItem>
                <MenuItem value="daily">{t('dashboard:filters.timeRanges.daily')}</MenuItem>
                <MenuItem value="weekly">{t('dashboard:filters.timeRanges.weekly')}</MenuItem>
              </Select>
            </FormControl>
          </Box>

          {loading ? (
            <Box sx={{ width: '100%' }}>
              <Skeleton variant="rectangular" height={400} />
            </Box>
          ) : (
            <Card sx={{ width: '100%' }}>
              <CardContent>
                <Typography variant="h6" sx={{ mb: 2, textAlign: 'center' }}>
                  {filters.sensorId && sensors.find(s => s.id === filters.sensorId)?.type.name
                    ? t(`dashboard:charts.${sensors.find(s => s.id === filters.sensorId)?.type.name}.title`)
                    : ''}
                </Typography>
                <Box sx={{ height: 400 }}>
                  {chartData ? (
                    <Line data={chartData} options={chartOptions} />
                  ) : (
                    <Box 
                      sx={{ 
                        height: '100%', 
                        display: 'flex', 
                        alignItems: 'center', 
                        justifyContent: 'center',
                        backgroundColor: 'rgba(0, 0, 0, 0.02)',
                        borderRadius: 1,
                      }}
                    >
                      <Typography variant="h5" color="text.secondary">
                        {t('dashboard:errors.noData')}
                      </Typography>
                    </Box>
                  )}
                </Box>
              </CardContent>
            </Card>
          )}
        </Box>
      </Box>

      <Snackbar
        open={!!error}
        autoHideDuration={6000}
        onClose={() => setError(null)}
        anchorOrigin={{ vertical: 'bottom', horizontal: 'center' }}
      >
        <Alert severity="error" onClose={() => setError(null)}>
          {error}
        </Alert>
      </Snackbar>
    </Box>
  );
};

export default DashboardPage; 
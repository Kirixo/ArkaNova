import React, { useState, useEffect, useCallback } from 'react';
import { useTranslation } from 'react-i18next';
import {
  Box,
  Typography,
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  FormControl,
  InputLabel,
  Select,
  MenuItem,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  IconButton,
  Snackbar,
  Alert,
  Skeleton,
  Container,
} from '@mui/material';
import { Delete as DeleteIcon } from '@mui/icons-material';
import { format } from 'date-fns';
import { enUS, uk } from 'date-fns/locale';
import Navigation from '../components/Navigation';
import { API_CONFIG } from '../config/api';
import type { Sensor, SensorType, SolarPanel, LatestMeasurement } from '../types/api';

interface DeleteDialogProps {
  open: boolean;
  sensorId: number | null;
  onConfirm: () => void;
  onCancel: () => void;
}

const DEFAULT_SENSOR_TYPES: SensorType[] = [
  { id: 1, name: 'temperature' },
  { id: 2, name: 'power' }
];

const DeleteDialog: React.FC<DeleteDialogProps> = ({ open, sensorId, onConfirm, onCancel }) => {
  const { t } = useTranslation(['sensors']);
  
  return (
    <Dialog open={open} onClose={onCancel}>
      <DialogTitle>{t('sensors:deleteDialog.title')}</DialogTitle>
      <DialogContent>
        <Typography>
          {t('sensors:deleteDialog.message', { id: sensorId })}
        </Typography>
      </DialogContent>
      <DialogActions>
        <Button onClick={onCancel} color="inherit">
          {t('sensors:deleteDialog.cancel')}
        </Button>
        <Button onClick={onConfirm} color="error" variant="contained">
          {t('sensors:deleteDialog.confirm')}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

const SensorsPage: React.FC = () => {
  const { t, i18n } = useTranslation(['sensors', 'common']);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [sensors, setSensors] = useState<Sensor[]>([]);
  const [panels, setPanels] = useState<SolarPanel[]>([]);
  const [sensorTypes, setSensorTypes] = useState<SensorType[]>(DEFAULT_SENSOR_TYPES);
  const [latestMeasurements, setLatestMeasurements] = useState<{ [key: number]: LatestMeasurement }>({});
  const [openAddDialog, setOpenAddDialog] = useState(false);
  const [openDeleteDialog, setOpenDeleteDialog] = useState(false);
  const [selectedSensorId, setSelectedSensorId] = useState<number | null>(null);
  const [newSensor, setNewSensor] = useState({
    typeId: '',
    panelId: '',
  });
  const user = JSON.parse(localStorage.getItem('user') || '{}');

  const formatDate = useCallback((timestamp: number) => {
    return format(new Date(timestamp), 'PPpp', {
      locale: i18n.language === 'uk-UA' ? uk : enUS,
    });
  }, [i18n.language]);

  const loadLatestMeasurements = useCallback(async (sensorIds: number[]) => {
    const measurements: { [key: number]: LatestMeasurement } = {};
    
    await Promise.all(
      sensorIds.map(async (sensorId) => {
        try {
          const response = await fetch(
            `${API_CONFIG.BASE_URL}/api/measurement/latest/sensor?sensor_id=${sensorId}`,
            {
              headers: {
                'Content-Type': 'application/json',
                'ngrok-skip-browser-warning': 'true',
              },
              credentials: 'include',
            }
          );

          if (response.ok) {
            const data = await response.json();
            measurements[sensorId] = data;
          }
        } catch (err) {
          console.error(`Failed to load measurement for sensor ${sensorId}:`, err);
        }
      })
    );

    setLatestMeasurements(measurements);
  }, []);

  const loadSensors = useCallback(async () => {
    setLoading(true);
    try {
      // Load panels first
      const panelsResponse = await fetch(
        `${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.SOLAR_PANELS}?user_id=${user.id}&page=1&limit=100`,
        {
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (!panelsResponse.ok) {
        throw new Error('Failed to load panels');
      }

      const panelsData = await panelsResponse.json();
      setPanels(panelsData.panels || []);

      // Load all sensors for each panel
      const allSensors: Sensor[] = [];
      for (const panel of panelsData.panels || []) {
        const sensorsResponse = await fetch(
          `${API_CONFIG.BASE_URL}/api/sensor/list/solarpanel?panel_id=${panel.id}&page=1&limit=100`,
          {
            headers: {
              'Content-Type': 'application/json',
              'ngrok-skip-browser-warning': 'true',
            },
            credentials: 'include',
          }
        );

        if (sensorsResponse.ok) {
          const sensorsData = await sensorsResponse.json();
          allSensors.push(...(sensorsData.sensors || []));
        }
      }

      // Sort sensors by type (locale-specific)
      const sortedSensors = [...allSensors].sort((a, b) => 
        a.type.name.localeCompare(b.type.name, i18n.language)
      );

      setSensors(sortedSensors);
      
      // Load latest measurements for all sensors
      await loadLatestMeasurements(sortedSensors.map(s => s.id));
    } catch (err) {
      console.error('Failed to load sensors:', err);
      setError(t('sensors:errors.loadFailed'));
    } finally {
      setLoading(false);
    }
  }, [t, user.id, i18n.language, loadLatestMeasurements]);

  // Load sensor types
  useEffect(() => {
    const loadSensorTypes = async () => {
      try {
        const response = await fetch(
          `${API_CONFIG.BASE_URL}/api/sensor/types`,
          {
            headers: {
              'Content-Type': 'application/json',
              'ngrok-skip-browser-warning': 'true',
            },
            credentials: 'include',
          }
        );

        if (response.ok) {
          const data = await response.json();
          setSensorTypes(data.types || []);
        }
      } catch (err) {
        console.error('Failed to load sensor types:', err);
      }
    };

    loadSensorTypes();
  }, []);

  // Load initial data
  useEffect(() => {
    loadSensors();
  }, [loadSensors]);

  const handleAddSensor = async () => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/sensor`,
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
          body: JSON.stringify({
            type_id: Number(newSensor.typeId),
            solar_panel_id: Number(newSensor.panelId),
          }),
        }
      );

      if (!response.ok) {
        throw new Error('Failed to add sensor');
      }

      setOpenAddDialog(false);
      setNewSensor({ typeId: '', panelId: '' });
      await loadSensors();
    } catch (err) {
      console.error('Failed to add sensor:', err);
      setError(t('sensors:errors.addFailed'));
    }
  };

  const handleDeleteSensor = async () => {
    if (!selectedSensorId) return;

    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/sensor?id=${selectedSensorId}`,
        {
          method: 'DELETE',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (!response.ok) {
        throw new Error('Failed to delete sensor');
      }

      setOpenDeleteDialog(false);
      setSelectedSensorId(null);
      await loadSensors();
    } catch (err) {
      console.error('Failed to delete sensor:', err);
      setError(t('sensors:errors.deleteFailed'));
    }
  };

  return (
    <Box sx={{ display: 'flex' }}>
      <Navigation isAdmin={user.role === 'admin'} />
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          p: 3,
          backgroundColor: '#F1F1F1',
          minHeight: '100vh',
        }}
      >
        <Container maxWidth={false}>
          {/* Header */}
          <Box sx={{ mb: 4, textAlign: 'center' }}>
            <Typography
              variant="h1"
              sx={{
                fontSize: '32px',
                fontWeight: 700,
                fontFamily: 'Roboto',
                mb: 1,
              }}
            >
              {t('sensors:title')}
            </Typography>
            <Typography
              variant="subtitle1"
              color="text.secondary"
              sx={{ mb: 3 }}
            >
              {t('sensors:subtitle')}
            </Typography>
            <Button
              variant="contained"
              color="primary"
              onClick={() => setOpenAddDialog(true)}
              disabled={panels.length === 0}
              sx={{ minWidth: 200 }}
            >
              {t('sensors:buttons.addSensor')}
            </Button>
          </Box>

          {/* Table */}
          <TableContainer component={Paper}>
            <Table>
              <TableHead>
                <TableRow>
                  <TableCell>{t('sensors:table.id')}</TableCell>
                  <TableCell>{t('sensors:table.type')}</TableCell>
                  <TableCell>{t('sensors:table.panelId')}</TableCell>
                  <TableCell>{t('sensors:table.panelName')}</TableCell>
                  <TableCell>{t('sensors:table.latestData')}</TableCell>
                  <TableCell>{t('sensors:table.lastUpdated')}</TableCell>
                  <TableCell align="right">{t('sensors:table.actions')}</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {loading ? (
                  // Skeleton rows
                  Array.from(new Array(5)).map((_, index) => (
                    <TableRow key={index}>
                      {Array.from(new Array(7)).map((_, cellIndex) => (
                        <TableCell key={cellIndex}>
                          <Skeleton animation="wave" />
                        </TableCell>
                      ))}
                    </TableRow>
                  ))
                ) : sensors.length > 0 ? (
                  sensors.map((sensor) => {
                    const latestMeasurement = latestMeasurements[sensor.id];
                    const panel = panels.find(p => p.id === sensor.solar_panel_id);
                    return (
                      <TableRow key={sensor.id}>
                        <TableCell>{sensor.id}</TableCell>
                        <TableCell>{t(`sensors:types.${sensor.type.name}`)}</TableCell>
                        <TableCell>{sensor.solar_panel_id}</TableCell>
                        <TableCell>
                          {panel ? panel.location : t('sensors:table.unknownPanel')}
                        </TableCell>
                        <TableCell>
                          {latestMeasurement ? (
                            <Typography>
                              {latestMeasurement.data} {t(`sensors:units.${sensor.type.name}`)}
                            </Typography>
                          ) : (
                            <Typography color="text.secondary">
                              {t('sensors:table.noData')}
                            </Typography>
                          )}
                        </TableCell>
                        <TableCell>
                          {latestMeasurement ? (
                            formatDate(latestMeasurement.recorded_at)
                          ) : (
                            t('sensors:table.never')
                          )}
                        </TableCell>
                        <TableCell align="right">
                          <IconButton
                            color="error"
                            onClick={() => {
                              setSelectedSensorId(sensor.id);
                              setOpenDeleteDialog(true);
                            }}
                          >
                            <DeleteIcon />
                          </IconButton>
                        </TableCell>
                      </TableRow>
                    );
                  })
                ) : (
                  <TableRow>
                    <TableCell colSpan={7} align="center">
                      {t('sensors:table.noSensors')}
                    </TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        </Container>
      </Box>

      {/* Add Sensor Dialog */}
      <Dialog open={openAddDialog} onClose={() => setOpenAddDialog(false)}>
        <DialogTitle>{t('sensors:addDialog.title')}</DialogTitle>
        <DialogContent>
          <Box sx={{ width: 400, mt: 2 }}>
            <FormControl fullWidth sx={{ mb: 2 }}>
              <InputLabel>{t('sensors:addDialog.type')}</InputLabel>
              <Select
                value={newSensor.typeId}
                label={t('sensors:addDialog.type')}
                onChange={(e) => setNewSensor({ ...newSensor, typeId: e.target.value })}
              >
                {sensorTypes.map((type) => (
                  <MenuItem key={type.id} value={type.id}>
                    {t(`sensors:types.${type.name}`)}
                  </MenuItem>
                ))}
              </Select>
            </FormControl>
            <FormControl fullWidth>
              <InputLabel>{t('sensors:addDialog.panel')}</InputLabel>
              <Select
                value={newSensor.panelId}
                label={t('sensors:addDialog.panel')}
                onChange={(e) => setNewSensor({ ...newSensor, panelId: e.target.value })}
              >
                {panels.map((panel) => (
                  <MenuItem key={panel.id} value={panel.id}>
                    {t('sensors:addDialog.panelName', { id: panel.id, location: panel.location })}
                  </MenuItem>
                ))}
              </Select>
            </FormControl>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setOpenAddDialog(false)} color="inherit">
            {t('sensors:addDialog.cancel')}
          </Button>
          <Button
            onClick={handleAddSensor}
            variant="contained"
            disabled={!newSensor.typeId || !newSensor.panelId}
          >
            {t('sensors:addDialog.save')}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Delete Confirmation Dialog */}
      <DeleteDialog
        open={openDeleteDialog}
        sensorId={selectedSensorId}
        onConfirm={handleDeleteSensor}
        onCancel={() => {
          setOpenDeleteDialog(false);
          setSelectedSensorId(null);
        }}
      />

      {/* Error Snackbar */}
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

export default SensorsPage; 
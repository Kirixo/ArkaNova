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
  TextField,
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
import { Edit as EditIcon, Delete as DeleteIcon } from '@mui/icons-material';
import { format } from 'date-fns';
import { formatInTimeZone } from 'date-fns-tz';
import { enUS, uk } from 'date-fns/locale';
import Navigation from '../components/Navigation';
import { API_CONFIG } from '../config/api';
import type { SolarPanel } from '../types/api';

interface DeleteDialogProps {
  open: boolean;
  panelId: number | null;
  onConfirm: () => void;
  onCancel: () => void;
}

interface PanelDialogProps {
  open: boolean;
  panel: SolarPanel | null;
  onSave: (location: string) => void;
  onCancel: () => void;
}

const DeleteDialog: React.FC<DeleteDialogProps> = ({ open, panelId, onConfirm, onCancel }) => {
  const { t } = useTranslation(['panels']);
  
  return (
    <Dialog open={open} onClose={onCancel}>
      <DialogTitle>{t('deleteDialog.title')}</DialogTitle>
      <DialogContent>
        <Typography>
          {t('deleteDialog.message', { id: panelId })}
        </Typography>
      </DialogContent>
      <DialogActions>
        <Button onClick={onCancel} color="inherit">
          {t('deleteDialog.cancel')}
        </Button>
        <Button onClick={onConfirm} color="error" variant="contained">
          {t('deleteDialog.confirm')}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

const PanelDialog: React.FC<PanelDialogProps> = ({ open, panel, onSave, onCancel }) => {
  const { t } = useTranslation(['panels']);
  const [location, setLocation] = useState('');
  const [error, setError] = useState('');

  useEffect(() => {
    if (panel) {
      setLocation(panel.location);
    } else {
      setLocation('');
    }
    setError('');
  }, [panel]);

  const handleSave = () => {
    if (!location.trim()) {
      setError(t('errors.locationRequired'));
      return;
    }
    onSave(location.trim());
  };

  return (
    <Dialog open={open} onClose={onCancel}>
      <DialogTitle>
        {panel ? t('editDialog.title') : t('addDialog.title')}
      </DialogTitle>
      <DialogContent>
        <Box sx={{ width: 400, mt: 2 }}>
          <TextField
            fullWidth
            label={t('addDialog.location')}
            placeholder={t('addDialog.locationPlaceholder')}
            value={location}
            onChange={(e) => {
              setLocation(e.target.value);
              setError('');
            }}
            error={!!error}
            helperText={error}
          />
        </Box>
      </DialogContent>
      <DialogActions>
        <Button onClick={onCancel} color="inherit">
          {t('addDialog.cancel')}
        </Button>
        <Button onClick={handleSave} variant="contained" disabled={!location.trim()}>
          {t('addDialog.save')}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

const SolarPanelsPage: React.FC = () => {
  const { t, i18n } = useTranslation(['panels']);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [panels, setPanels] = useState<SolarPanel[]>([]);
  const [openAddDialog, setOpenAddDialog] = useState(false);
  const [openEditDialog, setOpenEditDialog] = useState(false);
  const [openDeleteDialog, setOpenDeleteDialog] = useState(false);
  const [selectedPanel, setSelectedPanel] = useState<SolarPanel | null>(null);
  const user = JSON.parse(localStorage.getItem('user') || '{}');

  const formatDate = useCallback((timestamp: number) => {
    try {
      // Debug logging
      console.log('Original timestamp:', timestamp);
      console.log('Timestamp type:', typeof timestamp);
      
      // Check if timestamp is in milliseconds (13 digits) or seconds (10 digits)
      const timestampMs = timestamp.toString().length === 10 ? timestamp * 1000 : timestamp;
      console.log('Converted timestamp:', timestampMs);
      
      // Create date object
      const date = new Date(timestampMs);
      console.log('Date object:', date.toISOString());
      
      if (isNaN(date.getTime())) {
        console.error('Invalid timestamp:', timestamp);
        return 'Invalid date';
      }

      // Get user's timezone
      const userTimeZone = Intl.DateTimeFormat().resolvedOptions().timeZone;
      console.log('User timezone:', userTimeZone);

      // Format in user's timezone
      const formattedDate = formatInTimeZone(
        date,
        userTimeZone,
        i18n.language === 'uk-UA' ? 'd MMM yyyy р., HH:mm' : 'MMM d, yyyy, h:mm a',
        { locale: i18n.language === 'uk-UA' ? uk : enUS }
      );
      console.log('Formatted date:', formattedDate);
      
      return formattedDate;
    } catch (err) {
      console.error('Error formatting date:', err);
      return 'Invalid date';
    }
  }, [i18n.language]);

  const loadPanels = useCallback(async () => {
    setLoading(true);
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.SOLAR_PANELS}?user_id=${user.id}&page=1&limit=100`,
        {
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (!response.ok) {
        throw new Error('Failed to load panels');
      }

      const data = await response.json();
      console.log('Loaded panels data:', data); // Debug log
      
      // Sort panels by location (locale-specific)
      const sortedPanels = [...(data.panels || [])].sort((a, b) => 
        a.location.localeCompare(b.location, i18n.language)
      );

      console.log('Sorted panels:', sortedPanels); // Debug log
      setPanels(sortedPanels);
    } catch (err) {
      console.error('Failed to load panels:', err);
      setError(t('errors.loadFailed'));
    } finally {
      setLoading(false);
    }
  }, [t, user.id, i18n.language]);

  useEffect(() => {
    loadPanels();
  }, [loadPanels]);

  const handleAddPanel = async (location: string) => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/solarpanel`,
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
          body: JSON.stringify({
            user_id: user.id,
            location,
          }),
        }
      );

      if (!response.ok) {
        throw new Error('Failed to add panel');
      }

      setOpenAddDialog(false);
      await loadPanels();
    } catch (err) {
      console.error('Failed to add panel:', err);
      setError(t('errors.saveFailed'));
    }
  };

  const handleEditPanel = async (location: string) => {
    if (!selectedPanel) return;

    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/solarpanel?id=${selectedPanel.id}`,
        {
          method: 'PATCH',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
          body: JSON.stringify({
            location,
          }),
        }
      );

      if (!response.ok) {
        throw new Error('Failed to update panel');
      }

      setOpenEditDialog(false);
      setSelectedPanel(null);
      await loadPanels();
    } catch (err) {
      console.error('Failed to update panel:', err);
      setError(t('errors.saveFailed'));
    }
  };

  const handleDeletePanel = async () => {
    if (!selectedPanel) return;

    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/solarpanel?id=${selectedPanel.id}`,
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
        throw new Error('Failed to delete panel');
      }

      setOpenDeleteDialog(false);
      setSelectedPanel(null);
      await loadPanels();
    } catch (err) {
      console.error('Failed to delete panel:', err);
      setError(t('errors.deleteFailed'));
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
                fontSize: '24px',
                fontWeight: 700,
                fontFamily: 'Roboto',
                mb: 1,
              }}
            >
              {t('title')}
            </Typography>
            <Typography
              variant="subtitle1"
              color="text.secondary"
              sx={{ mb: 3 }}
            >
              {t('subtitle')}
            </Typography>
            <Button
              variant="contained"
              color="primary"
              onClick={() => setOpenAddDialog(true)}
              sx={{ 
                minWidth: 200,
                backgroundColor: '#4CAF50',
                '&:hover': {
                  backgroundColor: '#45a049',
                }
              }}
            >
              {t('buttons.addPanel')}
            </Button>
          </Box>

          {/* Table */}
          <TableContainer component={Paper}>
            <Table>
              <TableHead>
                <TableRow>
                  <TableCell>{t('table.id')}</TableCell>
                  <TableCell>{t('table.location')}</TableCell>
                  <TableCell>{t('table.createdAt')}</TableCell>
                  <TableCell>{t('table.updatedAt')}</TableCell>
                  <TableCell align="right">{t('table.actions')}</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {loading ? (
                  // Skeleton rows
                  Array.from(new Array(5)).map((_, index) => (
                    <TableRow key={index}>
                      {Array.from(new Array(5)).map((_, cellIndex) => (
                        <TableCell key={cellIndex}>
                          <Skeleton animation="wave" />
                        </TableCell>
                      ))}
                    </TableRow>
                  ))
                ) : panels.length > 0 ? (
                  panels.map((panel) => (
                    <TableRow key={panel.id}>
                      <TableCell>{panel.id}</TableCell>
                      <TableCell>{panel.location}</TableCell>
                      <TableCell>
                        {panel.created_at ? formatDate(panel.created_at) : 'N/A'}
                      </TableCell>
                      <TableCell>
                        {panel.updated_at ? formatDate(panel.updated_at) : 'N/A'}
                      </TableCell>
                      <TableCell align="right">
                        <IconButton
                          color="primary"
                          onClick={() => {
                            setSelectedPanel(panel);
                            setOpenEditDialog(true);
                          }}
                          sx={{ color: '#2196F3' }}
                        >
                          <EditIcon />
                        </IconButton>
                        <IconButton
                          color="error"
                          onClick={() => {
                            setSelectedPanel(panel);
                            setOpenDeleteDialog(true);
                          }}
                          sx={{ color: '#F44336' }}
                        >
                          <DeleteIcon />
                        </IconButton>
                      </TableCell>
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={5} align="center">
                      {t('table.noPanels')}
                    </TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        </Container>
      </Box>

      {/* Add Panel Dialog */}
      <PanelDialog
        open={openAddDialog}
        panel={null}
        onSave={handleAddPanel}
        onCancel={() => setOpenAddDialog(false)}
      />

      {/* Edit Panel Dialog */}
      <PanelDialog
        open={openEditDialog}
        panel={selectedPanel}
        onSave={handleEditPanel}
        onCancel={() => {
          setOpenEditDialog(false);
          setSelectedPanel(null);
        }}
      />

      {/* Delete Confirmation Dialog */}
      <DeleteDialog
        open={openDeleteDialog}
        panelId={selectedPanel?.id || null}
        onConfirm={handleDeletePanel}
        onCancel={() => {
          setOpenDeleteDialog(false);
          setSelectedPanel(null);
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

export default SolarPanelsPage; 
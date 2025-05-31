import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { useTranslation } from 'react-i18next';
import {
  Box,
  Typography,
  Card,
  CardContent,
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  IconButton,
  Skeleton,
  Snackbar,
  Alert,
  Container,
  Select,
  MenuItem,
  FormControl,
  InputLabel,
} from '@mui/material';
import { Edit as EditIcon } from '@mui/icons-material';
import Navigation from '../components/Navigation';
import { API_CONFIG } from '../config/api';
import { authService } from '../services/auth';

interface User {
  id: string;
  email: string;
}

interface EditDialogData {
  email: string;
  password: string;
}

const ProfilePage: React.FC = () => {
  const { t, i18n } = useTranslation(['profile', 'common']);
  const navigate = useNavigate();
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [user, setUser] = useState<User | null>(null);
  const [openEditDialog, setOpenEditDialog] = useState(false);
  const [openDeleteDialog, setOpenDeleteDialog] = useState(false);
  const [editData, setEditData] = useState<EditDialogData>({
    email: '',
    password: '',
  });

  useEffect(() => {
    const loadUser = async () => {
      try {
        const storedUser = authService.getUser();
        if (!storedUser) {
          throw new Error('No user found');
        }

        const response = await fetch(
          `${API_CONFIG.BASE_URL}/api/users?id=${storedUser.id}`,
          {
            headers: {
              'Content-Type': 'application/json',
              'ngrok-skip-browser-warning': 'true',
            },
            credentials: 'include',
          }
        );

        if (!response.ok) {
          throw new Error('Failed to load user data');
        }

        const userData = await response.json();
        setUser(userData);
        setEditData({ email: userData.email, password: '' });
      } catch (err) {
        console.error('Failed to load user:', err);
        setError(t('profile:errors.loadFailed'));
      } finally {
        setLoading(false);
      }
    };

    loadUser();
  }, [t]);

  const handleEditProfile = async () => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/users?id=${user?.id}`,
        {
          method: 'PATCH',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
          body: JSON.stringify({
            email: editData.email,
            ...(editData.password ? { password: editData.password } : {}),
          }),
        }
      );

      if (!response.ok) {
        throw new Error('Failed to update profile');
      }

      const updatedUser = await response.json();
      setUser(updatedUser);
      authService.setUser(updatedUser);
      setOpenEditDialog(false);
    } catch (err) {
      console.error('Failed to update profile:', err);
      setError(t('profile:errors.updateFailed'));
    }
  };

  const handleDeleteAccount = async () => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/users?id=${user?.id}`,
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
        throw new Error('Failed to delete account');
      }

      await authService.logout(navigate);
    } catch (err) {
      console.error('Failed to delete account:', err);
      setError(t('profile:errors.deleteFailed'));
    }
  };

  const handleLogout = async () => {
    await authService.logout(navigate);
  };

  const handleLanguageChange = (lang: string) => {
    i18n.changeLanguage(lang);
  };

  return (
    <Box sx={{ display: 'flex' }}>
      <Navigation />
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          p: 3,
          backgroundColor: '#F1F1F1',
          minHeight: '100vh',
        }}
      >
        <Container maxWidth="sm">
          {/* Header */}
          <Typography
            variant="h1"
            sx={{
              fontSize: '24px',
              fontWeight: 700,
              fontFamily: 'Roboto',
              mb: 3,
              textAlign: 'center',
            }}
          >
            {t('profile:title')}
          </Typography>

          {/* Profile Card */}
          <Card sx={{ mb: 3 }}>
            <CardContent>
              {loading ? (
                <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
                  <Skeleton variant="text" width="60%" height={32} />
                  <Skeleton variant="text" width="40%" height={24} />
                </Box>
              ) : (
                <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                  <Box>
                    <Typography variant="h6" gutterBottom>
                      {t('profile:email')}
                    </Typography>
                    <Typography color="text.secondary">
                      {user?.email}
                    </Typography>
                  </Box>
                  <IconButton
                    onClick={() => setOpenEditDialog(true)}
                    sx={{ color: '#2196F3' }}
                  >
                    <EditIcon />
                  </IconButton>
                </Box>
              )}
            </CardContent>
          </Card>

          {/* Language Selector */}
          <FormControl fullWidth sx={{ mb: 3 }}>
            <InputLabel>{t('profile:language')}</InputLabel>
            <Select
              value={i18n.language}
              label={t('profile:language')}
              onChange={(e) => handleLanguageChange(e.target.value)}
            >
              <MenuItem value="en-US">English</MenuItem>
              <MenuItem value="uk-UA">Українська</MenuItem>
            </Select>
          </FormControl>

          {/* Action Buttons */}
          <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
            <Button
              variant="contained"
              onClick={handleLogout}
              sx={{ backgroundColor: '#2196F3' }}
            >
              {t('profile:buttons.logout')}
            </Button>
            <Button
              variant="outlined"
              onClick={() => setOpenDeleteDialog(true)}
              sx={{ 
                color: '#F44336',
                borderColor: '#F44336',
                '&:hover': {
                  borderColor: '#d32f2f',
                  backgroundColor: 'rgba(244, 67, 54, 0.04)',
                },
              }}
            >
              {t('profile:buttons.deleteAccount')}
            </Button>
          </Box>
        </Container>
      </Box>

      {/* Edit Profile Dialog */}
      <Dialog open={openEditDialog} onClose={() => setOpenEditDialog(false)}>
        <DialogTitle>{t('profile:editDialog.title')}</DialogTitle>
        <DialogContent>
          <Box sx={{ pt: 2, width: 400 }}>
            <TextField
              fullWidth
              label={t('profile:editDialog.email')}
              value={editData.email}
              onChange={(e) => setEditData({ ...editData, email: e.target.value })}
              sx={{ mb: 2 }}
            />
            <TextField
              fullWidth
              type="password"
              label={t('profile:editDialog.password')}
              placeholder={t('profile:editDialog.passwordPlaceholder')}
              value={editData.password}
              onChange={(e) => setEditData({ ...editData, password: e.target.value })}
            />
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setOpenEditDialog(false)} color="inherit">
            {t('profile:editDialog.cancel')}
          </Button>
          <Button onClick={handleEditProfile} variant="contained">
            {t('profile:editDialog.save')}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Delete Account Dialog */}
      <Dialog open={openDeleteDialog} onClose={() => setOpenDeleteDialog(false)}>
        <DialogTitle>{t('profile:deleteDialog.title')}</DialogTitle>
        <DialogContent>
          <Typography>
            {t('profile:deleteDialog.message')}
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setOpenDeleteDialog(false)} color="inherit">
            {t('profile:deleteDialog.cancel')}
          </Button>
          <Button onClick={handleDeleteAccount} color="error" variant="contained">
            {t('profile:deleteDialog.confirm')}
          </Button>
        </DialogActions>
      </Dialog>

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

export default ProfilePage; 
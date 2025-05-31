import React, { useState } from 'react';
import { useTranslation } from 'react-i18next';
import { useNavigate } from 'react-router-dom';
import {
  Box,
  Card,
  TextField,
  Button,
  Typography,
  IconButton,
  InputAdornment,
  Snackbar,
  Alert,
  Menu,
  MenuItem,
  CircularProgress,
} from '@mui/material';
import {
  Visibility,
  VisibilityOff,
  Language,
} from '@mui/icons-material';
import { apiClient, API_CONFIG } from '../config/api';
import type { UserResponse } from '../config/api';
import { authService } from '../services/auth';

const RegisterPage: React.FC = () => {
  const { t, i18n } = useTranslation();
  const navigate = useNavigate();
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [confirmPassword, setConfirmPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [showConfirmPassword, setShowConfirmPassword] = useState(false);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [languageAnchor, setLanguageAnchor] = useState<null | HTMLElement>(null);

  const isValidEmail = (email: string) => {
    if (!email || !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
      return false;
    }
    // Prevent @arkanova.com registrations
    if (email.toLowerCase().endsWith('@arkanova.com')) {
      return false;
    }
    return true;
  };

  const isValidPassword = (password: string) => {
    return password.length >= 8;
  };

  const handleRegister = async (e: React.FormEvent) => {
    e.preventDefault();
    console.log('Registration started');
    
    if (!isValidEmail(email)) {
      if (email.toLowerCase().endsWith('@arkanova.com')) {
        setError(t('register.adminEmailRestricted'));
      } else {
        setError(t('register.invalidEmail'));
      }
      return;
    }

    if (!isValidPassword(password)) {
      setError(t('register.passwordTooShort'));
      return;
    }

    if (password !== confirmPassword) {
      setError(t('register.passwordsDontMatch'));
      return;
    }

    setLoading(true);
    setError(null);

    try {
      console.log('Sending registration request...');
      const response = await fetch(`${API_CONFIG.BASE_URL}/api/users/register`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'ngrok-skip-browser-warning': 'true',
        },
        credentials: 'include',
        body: JSON.stringify({ email, password }),
      });

      console.log('Registration response:', response.status);
      const data = await response.json();
      console.log('Registration data:', data);

      if (!response.ok) {
        console.error('Registration failed:', data);
        setError(t('register.emailExists'));
        return;
      }

      if (data && data.id) {
        console.log('Setting user data in authService');
        authService.setUser(data);
        
        const redirectPath = email.endsWith('@arkanova.com') ? '/admin' : '/home';
        console.log('Redirecting to:', redirectPath);
        
        // Add a small delay to ensure state is updated
        setTimeout(() => {
          navigate(redirectPath, { replace: true });
        }, 100);
      } else {
        console.error('Invalid response data:', data);
        setError(t('register.noInternet'));
      }
    } catch (err) {
      console.error('Registration Error:', err);
      setError(t('register.noInternet'));
    } finally {
      setLoading(false);
    }
  };

  const handleLanguageChange = (lang: string) => {
    i18n.changeLanguage(lang);
    setLanguageAnchor(null);
  };

  return (
    <Box
      sx={{
        minHeight: '100vh',
        minWidth: '100vw',
        display: 'flex',
        flexDirection: 'column',
        background: 'linear-gradient(135deg, #4CAF50 0%, #F1F1F1 100%)',
        position: 'fixed',
        top: 0,
        left: 0,
        right: 0,
        bottom: 0,
        overflow: 'auto',
      }}
    >
      {/* Language Switcher */}
      <Box sx={{ position: 'absolute', top: 16, right: 16, zIndex: 2 }}>
        <IconButton
          onClick={(e) => setLanguageAnchor(e.currentTarget)}
          aria-label="Change language"
          sx={{
            backgroundColor: 'rgba(255, 255, 255, 0.1)',
            '&:hover': {
              backgroundColor: 'rgba(255, 255, 255, 0.2)',
            },
          }}
        >
          <Language />
        </IconButton>
        <Menu
          anchorEl={languageAnchor}
          open={Boolean(languageAnchor)}
          onClose={() => setLanguageAnchor(null)}
        >
          <MenuItem onClick={() => handleLanguageChange('en-US')}>English</MenuItem>
          <MenuItem onClick={() => handleLanguageChange('uk-UA')}>Українська</MenuItem>
        </Menu>
      </Box>

      {/* Main Content */}
      <Box
        sx={{
          flex: 1,
          display: 'flex',
          justifyContent: 'center',
          alignItems: 'center',
          p: 2,
          minHeight: '100vh',
        }}
      >
        <Card
          elevation={4}
          sx={{
            width: '100%',
            maxWidth: 400,
            p: { xs: 2, sm: 4 },
            background: 'rgba(255, 255, 255, 0.1)',
            backdropFilter: 'blur(10px)',
            border: '1px solid rgba(255, 255, 255, 0.2)',
          }}
        >
          {/* Logo */}
          <Box sx={{ textAlign: 'center', mb: 3 }}>
            <svg
              width="48"
              height="48"
              viewBox="0 0 48 48"
              fill="#4CAF50"
            >
              <rect width="48" height="48" rx="8" />
            </svg>
          </Box>

          {/* Title */}
          <Typography
            variant="h1"
            component="h1"
            sx={{
              textAlign: 'center',
              mb: 4,
              color: '#212121',
              fontSize: '24px',
              fontWeight: 700,
            }}
          >
            {t('register.title')}
          </Typography>

          {/* Form */}
          <form onSubmit={handleRegister}>
            <TextField
              fullWidth
              type="email"
              label={t('register.email')}
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              error={email !== '' && !isValidEmail(email)}
              helperText={email !== '' && !isValidEmail(email) ? t('register.invalidEmail') : ''}
              sx={{
                mb: 2,
                '& .MuiOutlinedInput-root': {
                  backgroundColor: 'rgba(255, 255, 255, 0.05)',
                },
                '& .MuiInputLabel-root': {
                  color: 'text.primary',
                },
              }}
              inputProps={{
                'aria-label': t('register.email'),
              }}
            />

            <TextField
              fullWidth
              type={showPassword ? 'text' : 'password'}
              label={t('register.password')}
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              error={password !== '' && !isValidPassword(password)}
              helperText={password !== '' && !isValidPassword(password) ? t('register.passwordTooShort') : ''}
              sx={{
                mb: 2,
                '& .MuiOutlinedInput-root': {
                  backgroundColor: 'rgba(255, 255, 255, 0.05)',
                },
                '& .MuiInputLabel-root': {
                  color: 'text.primary',
                },
              }}
              InputProps={{
                endAdornment: (
                  <InputAdornment position="end">
                    <IconButton
                      onClick={() => setShowPassword(!showPassword)}
                      edge="end"
                      aria-label={showPassword ? 'Hide password' : 'Show password'}
                    >
                      {showPassword ? <VisibilityOff /> : <Visibility />}
                    </IconButton>
                  </InputAdornment>
                ),
              }}
              inputProps={{
                'aria-label': t('register.password'),
              }}
            />

            <TextField
              fullWidth
              type={showConfirmPassword ? 'text' : 'password'}
              label={t('register.confirmPassword')}
              value={confirmPassword}
              onChange={(e) => setConfirmPassword(e.target.value)}
              error={confirmPassword !== '' && password !== confirmPassword}
              helperText={confirmPassword !== '' && password !== confirmPassword ? t('register.passwordsDontMatch') : ''}
              sx={{
                mb: 3,
                '& .MuiOutlinedInput-root': {
                  backgroundColor: 'rgba(255, 255, 255, 0.05)',
                },
                '& .MuiInputLabel-root': {
                  color: 'text.primary',
                },
              }}
              InputProps={{
                endAdornment: (
                  <InputAdornment position="end">
                    <IconButton
                      onClick={() => setShowConfirmPassword(!showConfirmPassword)}
                      edge="end"
                      aria-label={showConfirmPassword ? 'Hide confirm password' : 'Show confirm password'}
                    >
                      {showConfirmPassword ? <VisibilityOff /> : <Visibility />}
                    </IconButton>
                  </InputAdornment>
                ),
              }}
              inputProps={{
                'aria-label': t('register.confirmPassword'),
              }}
            />

            <Button
              fullWidth
              type="submit"
              variant="contained"
              color="primary"
              size="large"
              disabled={loading || !isValidEmail(email) || !isValidPassword(password) || password !== confirmPassword}
              sx={{
                height: '48px',
                mb: 2,
                transition: 'transform 0.3s',
                '&:hover': {
                  transform: 'scale(1.02)',
                },
              }}
            >
              {loading ? (
                <CircularProgress size={24} color="inherit" />
              ) : (
                t('register.registerButton')
              )}
            </Button>

            <Box sx={{ display: 'flex', justifyContent: 'center' }}>
              <Button
                color="secondary"
                onClick={() => navigate('/login')}
              >
                {t('register.backToLogin')}
              </Button>
            </Box>
          </form>
        </Card>
      </Box>

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

export default RegisterPage; 
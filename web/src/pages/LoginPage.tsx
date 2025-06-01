import React, { useState, useEffect } from 'react';
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

const LoginPage: React.FC = () => {
  const { t, i18n } = useTranslation();
  const navigate = useNavigate();
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [languageAnchor, setLanguageAnchor] = useState<null | HTMLElement>(null);

  useEffect(() => {
    console.log('LoginPage - Component mounted');
    if (authService.isAuthenticated()) {
      console.log('LoginPage - User already logged in, redirecting to home');
      navigate('/home', { replace: true });
    }
  }, [navigate]);

  const handleLogin = async (e: React.FormEvent) => {
    e.preventDefault();
    console.log('LoginPage - Login attempt started');
    setLoading(true);
    setError(null);

    try {
      // Attempt preflight first
      const preflightOk = await apiClient.preflight(API_CONFIG.ENDPOINTS.LOGIN);
      console.log('LoginPage - Preflight check:', preflightOk ? 'OK' : 'Failed');
      
      if (!preflightOk) {
        console.warn('LoginPage - Preflight failed, attempting direct request');
      }

      console.log('LoginPage - Sending login request');
      const response = await fetch(`${API_CONFIG.BASE_URL}${API_CONFIG.ENDPOINTS.LOGIN}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'ngrok-skip-browser-warning': 'true',
        },
        credentials: 'include',
        body: JSON.stringify({ email, password }),
      });

      console.log('LoginPage - Response status:', response.status);
      const data = await response.json();
      console.log('LoginPage - Response data:', data);

      if (!response.ok) {
        console.error('LoginPage - Login failed:', data);
        setError(t('login.invalidCredentials'));
        return;
      }

      if (data) {
        console.log('LoginPage - Login successful, saving user data');
        authService.setUser(data);
        
        const redirectPath = data.email?.endsWith('@arkanova.com') ? '/admin' : '/home';
        console.log('LoginPage - Redirecting to:', redirectPath);
        
        // Use replace to prevent going back to login page
        navigate(redirectPath, { replace: true });
      } else {
        console.error('LoginPage - No data in successful response');
        setError(t('login.invalidCredentials'));
      }
    } catch (err) {
      console.error('LoginPage - Login error:', err);
      setError(t('login.noInternet'));
    } finally {
      setLoading(false);
    }
  };

  const handleLanguageChange = (lang: string) => {
    i18n.changeLanguage(lang);
    setLanguageAnchor(null);
  };

  const isValidEmail = (email: string) => {
    return /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email);
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
            <img
              src="/android-logo.png"
              alt="ArkaNova Logo"
              style={{
                width: '48px',
                height: '48px',
                borderRadius: '8px',
              }}
            />
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
            {t('login.title')}
          </Typography>

          {/* Form */}
          <form onSubmit={handleLogin}>
            <TextField
              fullWidth
              type="email"
              label={t('login.email')}
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              error={email !== '' && !isValidEmail(email)}
              helperText={email !== '' && !isValidEmail(email) ? t('login.invalidEmail') : ''}
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
                'aria-label': t('login.email'),
              }}
            />

            <TextField
              fullWidth
              type={showPassword ? 'text' : 'password'}
              label={t('login.password')}
              value={password}
              onChange={(e) => setPassword(e.target.value)}
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
                'aria-label': t('login.password'),
              }}
            />

            <Button
              fullWidth
              type="submit"
              variant="contained"
              color="primary"
              size="large"
              disabled={loading || !isValidEmail(email) || !password}
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
                t('login.loginButton')
              )}
            </Button>

            <Box
              sx={{
                display: 'flex',
                justifyContent: 'center',
                mt: 2
              }}
            >
              <Button
                color="secondary"
                onClick={() => navigate('/register')}
              >
                {t('login.register')}
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

export default LoginPage; 
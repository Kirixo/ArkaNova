import { ThemeProvider } from '@mui/material/styles';
import CssBaseline from '@mui/material/CssBaseline';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { theme } from './theme/theme';
import './i18n/config';
import LoginPage from './pages/LoginPage';
import RegisterPage from './pages/RegisterPage';
import HomePage from './pages/HomePage';
import DashboardPage from './pages/DashboardPage';
import SensorsPage from './pages/SensorsPage';
import SolarPanelsPage from './pages/SolarPanelsPage';
import ProfilePage from './pages/ProfilePage';
import AdminDashboardPage from './pages/AdminDashboardPage';
import ProtectedRoute from './components/ProtectedRoute';
import { authService } from './services/auth';
import '@fontsource/roboto/300.css';
import '@fontsource/roboto/400.css';
import '@fontsource/roboto/500.css';
import '@fontsource/roboto/700.css';

function App() {
  // Check if user is authenticated
  const isAuthenticated = authService.isAuthenticated();
  const isAdmin = authService.isAdmin();
  console.log('App - Initial auth state:', isAuthenticated ? 'Authenticated' : 'Not authenticated');

  return (
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Router>
        <Routes>
          {/* Public routes */}
          <Route 
            path="/login" 
            element={
              isAuthenticated ? <Navigate to="/home" replace /> : <LoginPage />
            } 
          />
          <Route 
            path="/register" 
            element={
              isAuthenticated ? <Navigate to="/home" replace /> : <RegisterPage />
            } 
          />

          {/* Protected routes */}
          <Route
            path="/home"
            element={
              <ProtectedRoute>
                <HomePage />
              </ProtectedRoute>
            }
          />
          <Route
            path="/dashboard"
            element={
              <ProtectedRoute>
                <DashboardPage />
              </ProtectedRoute>
            }
          />
          <Route
            path="/sensors"
            element={
              <ProtectedRoute>
                <SensorsPage />
              </ProtectedRoute>
            }
          />
          <Route
            path="/panels"
            element={
              <ProtectedRoute>
                <SolarPanelsPage />
              </ProtectedRoute>
            }
          />
          <Route
            path="/profile"
            element={
              <ProtectedRoute>
                {isAdmin ? <Navigate to="/admin" replace /> : <ProfilePage />}
              </ProtectedRoute>
            }
          />

          {/* Admin routes */}
          <Route
            path="/admin"
            element={
              <ProtectedRoute>
                {isAdmin ? <AdminDashboardPage /> : <Navigate to="/home" replace />}
              </ProtectedRoute>
            }
          />

          {/* Default route */}
          <Route 
            path="/" 
            element={
              isAuthenticated 
                ? <Navigate to={isAdmin ? '/admin' : '/home'} replace /> 
                : <Navigate to="/login" replace />
            } 
          />

          {/* Catch all route */}
          <Route path="*" element={<Navigate to="/" replace />} />
        </Routes>
      </Router>
    </ThemeProvider>
  );
}

export default App;

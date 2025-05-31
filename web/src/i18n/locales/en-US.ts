import dashboard from '../../locales/en/dashboard.json';
import sensors from '../../locales/en/sensors.json';
import panels from '../../locales/en/panels.json';
import profile from '../../locales/en/profile.json';
import admin from '../locales/en-US/admin.json';

export const enUS = {
  dashboard,
  sensors,
  panels,
  profile,
  admin,
  translation: {
    login: {
      title: 'Login',
      email: 'Email',
      password: 'Password',
      loginButton: 'Login',
      register: 'Register',
      forgotPassword: 'Forgot Password',
      invalidCredentials: 'Invalid credentials',
      noInternet: 'No internet connection',
      featureComingSoon: 'Feature coming soon',
      emailPlaceholder: 'Enter your email',
      passwordPlaceholder: 'Enter your password',
      invalidEmail: 'Invalid email format'
    },
    register: {
      title: 'Create Account',
      email: 'Email',
      password: 'Password',
      confirmPassword: 'Confirm Password',
      registerButton: 'Register',
      backToLogin: 'Back to Login',
      emailExists: 'Email already exists',
      noInternet: 'No internet connection',
      passwordTooShort: 'Password must be at least 8 characters',
      passwordsDontMatch: 'Passwords do not match',
      invalidEmail: 'Invalid email format',
      adminEmailRestricted: 'Registration with @arkanova.com email is restricted',
      success: 'Registration successful'
    },
    home: {
      greeting: 'Hi, {{email}}',
      temperature: {
        title: 'Temperature',
        status: {
          normal: 'Normal',
          warning: 'Warning',
          error: 'Critical'
        },
        lastUpdated: 'Last updated: {{time}}'
      },
      energy: {
        title: 'Energy Production',
        unit: 'kWh',
        lastUpdated: 'Last updated: {{time}}'
      },
      buttons: {
        viewDashboard: 'View Dashboard',
        viewSensors: 'View Sensors'
      },
      errors: {
        failedMetrics: 'Failed to load metrics',
        noPanels: 'No solar panels found'
      }
    },
    navigation: {
      home: 'Home',
      dashboard: 'Dashboard',
      sensors: 'Sensors',
      solarPanels: 'Solar Panels',
      profile: 'Profile',
      logout: 'Logout',
      userManagement: 'User Management'
    },
    common: {
      loading: 'Loading...',
      error: 'Error',
      success: 'Success'
    }
  }
}; 
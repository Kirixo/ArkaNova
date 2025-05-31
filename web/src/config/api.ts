import type { ApiResponse } from '../types/api';

const isDevelopment = import.meta.env.DEV;

const defaultHeaders = {
  'Content-Type': 'application/json',
  'ngrok-skip-browser-warning': 'true', // Skip ngrok warning
};

export const API_CONFIG = {
  BASE_URL: isDevelopment ? '' : 'https://literate-vastly-pony.ngrok-free.app',
  ENDPOINTS: {
    LOGIN: '/api/users/login',
    REGISTER: '/api/users/register',
    MEASUREMENTS: '/api/measurement/list/sensor',
    SOLAR_PANELS: '/api/solarpanel/list/user',
    // Add other endpoints as needed
  }
};

// API response types
export interface UserResponse {
  id: string;
  email: string;
  role?: string;
  // Add other user properties as needed
}

// API client with error handling
export const apiClient = {
  async post<T>(endpoint: string, data: any): Promise<ApiResponse<T>> {
    try {
      const response = await fetch(`${API_CONFIG.BASE_URL}${endpoint}`, {
        method: 'POST',
        headers: defaultHeaders,
        body: JSON.stringify(data),
        credentials: 'include',
      });

      if (!response.ok) {
        return { error: 'invalidCredentials' };
      }

      return await response.json();
    } catch (error) {
      console.error('API Error:', error);
      return { error: 'networkError' };
    }
  },

  async get<T>(endpoint: string): Promise<ApiResponse<T>> {
    try {
      const response = await fetch(`${API_CONFIG.BASE_URL}${endpoint}`, {
        method: 'GET',
        headers: defaultHeaders,
        credentials: 'include',
      });

      if (!response.ok) {
        return { error: 'requestFailed' };
      }

      return await response.json();
    } catch (error) {
      console.error('API Error:', error);
      return { error: 'networkError' };
    }
  },

  async preflight(endpoint: string): Promise<boolean> {
    try {
      const response = await fetch(`${API_CONFIG.BASE_URL}${endpoint}`, {
        method: 'OPTIONS',
        headers: defaultHeaders,
        credentials: 'include',
      });
      return response.ok;
    } catch (error) {
      console.error('Preflight Error:', error);
      return false;
    }
  }
}; 
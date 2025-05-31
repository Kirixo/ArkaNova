import type { NavigateFunction } from 'react-router-dom';

interface User {
  id: string;
  email: string;
  role?: string;
}

class AuthService {
  private static instance: AuthService;
  private logoutInProgress: boolean = false;

  private constructor() {}

  public static getInstance(): AuthService {
    if (!AuthService.instance) {
      AuthService.instance = new AuthService();
    }
    return AuthService.instance;
  }

  public getUser(): User | null {
    const userStr = localStorage.getItem('user');
    if (!userStr) return null;
    try {
      const user = JSON.parse(userStr);
      console.log('AuthService: Getting user', user);
      return user;
    } catch {
      console.error('AuthService: Failed to parse user data');
      return null;
    }
  }

  public isAuthenticated(): boolean {
    const isAuth = !!this.getUser();
    console.log('AuthService: Checking authentication:', isAuth);
    return isAuth;
  }

  public isAdmin(): boolean {
    const user = this.getUser();
    return user?.email?.endsWith('@arkanova.com') ?? false;
  }

  public async logout(navigate: NavigateFunction): Promise<void> {
    // Prevent multiple simultaneous logout attempts
    if (this.logoutInProgress) {
      return;
    }

    this.logoutInProgress = true;

    try {
      // Clear user data
      localStorage.removeItem('user');

      // Add a small delay to prevent race conditions
      await new Promise(resolve => setTimeout(resolve, 100));

      // Navigate to login with replace to prevent back navigation
      navigate('/login', { replace: true });
    } finally {
      // Reset the flag after a short delay
      setTimeout(() => {
        this.logoutInProgress = false;
      }, 500);
    }
  }

  public setUser(user: User): void {
    console.log('AuthService: Setting user', user);
    localStorage.setItem('user', JSON.stringify(user));
  }
}

export const authService = AuthService.getInstance(); 
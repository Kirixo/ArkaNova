import React, { useState } from 'react';
import { useTranslation } from 'react-i18next';
import { useNavigate, useLocation } from 'react-router-dom';
import {
  AppBar,
  Box,
  Drawer,
  IconButton,
  List,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Toolbar,
  Typography,
  Menu,
  MenuItem,
} from '@mui/material';
import {
  Menu as MenuIcon,
  Home,
  Dashboard,
  Sensors,
  SolarPower,
  Person,
  Logout,
  People,
  Language,
} from '@mui/icons-material';
import { authService } from '../services/auth';

interface NavigationProps {
  isAdmin?: boolean;
}

const Navigation: React.FC<NavigationProps> = ({ isAdmin = false }) => {
  const { t, i18n } = useTranslation();
  const navigate = useNavigate();
  const location = useLocation();
  const [drawerOpen, setDrawerOpen] = useState(false);
  const [languageAnchor, setLanguageAnchor] = useState<null | HTMLElement>(null);

  const menuItems = [
    ...(isAdmin 
      ? [{ path: '/admin', icon: <People />, label: 'navigation.userManagement' }]
      : [
          { path: '/home', icon: <Home />, label: 'navigation.home' },
          { path: '/dashboard', icon: <Dashboard />, label: 'navigation.dashboard' },
          { path: '/sensors', icon: <Sensors />, label: 'navigation.sensors' },
          { path: '/panels', icon: <SolarPower />, label: 'navigation.solarPanels' },
          { path: '/profile', icon: <Person />, label: 'navigation.profile' }
        ]
    ),
  ];

  const handleLanguageChange = (lang: string) => {
    i18n.changeLanguage(lang);
    setLanguageAnchor(null);
  };

  const handleLogout = async () => {
    await authService.logout(navigate);
  };

  const toggleDrawer = (open: boolean) => (event: React.KeyboardEvent | React.MouseEvent) => {
    if (
      event.type === 'keydown' &&
      ((event as React.KeyboardEvent).key === 'Tab' || (event as React.KeyboardEvent).key === 'Shift')
    ) {
      return;
    }
    setDrawerOpen(open);
  };

  return (
    <>
      <AppBar
        position="fixed"
        sx={{
          backgroundColor: '#4CAF50',
          color: '#FFFFFF',
          boxShadow: 'none',
          borderBottom: '1px solid rgba(0, 0, 0, 0.12)',
          zIndex: (theme) => theme.zIndex.drawer + 1,
        }}
      >
        <Toolbar>
          <IconButton
            color="inherit"
            aria-label="open drawer"
            edge="start"
            onClick={toggleDrawer(true)}
            sx={{ mr: 2 }}
          >
            <MenuIcon />
          </IconButton>
          <Typography variant="h6" noWrap component="div" sx={{ flexGrow: 1 }}>
            ArkaNova
          </Typography>
          <IconButton
            onClick={(e) => setLanguageAnchor(e.currentTarget)}
            aria-label="Change language"
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
        </Toolbar>
      </AppBar>

      <Drawer
        anchor="left"
        open={drawerOpen}
        onClose={toggleDrawer(false)}
        sx={{
          width: 240,
          flexShrink: 0,
          '& .MuiDrawer-paper': {
            width: 240,
            boxSizing: 'border-box',
            backgroundColor: '#F1F1F1',
          },
        }}
      >
        <Toolbar />
        <Box sx={{ overflow: 'auto' }}>
          <List>
            {menuItems.map((item) => (
              <ListItem key={item.path} disablePadding>
                <ListItemButton
                  selected={location.pathname === item.path}
                  onClick={() => {
                    navigate(item.path);
                    setDrawerOpen(false);
                  }}
                >
                  <ListItemIcon sx={{ color: location.pathname === item.path ? 'primary.main' : 'inherit' }}>
                    {item.icon}
                  </ListItemIcon>
                  <ListItemText primary={t(item.label)} />
                </ListItemButton>
              </ListItem>
            ))}
            <ListItem disablePadding>
              <ListItemButton onClick={handleLogout}>
                <ListItemIcon>
                  <Logout />
                </ListItemIcon>
                <ListItemText primary={t('navigation.logout')} />
              </ListItemButton>
            </ListItem>
          </List>
        </Box>
      </Drawer>
      <Toolbar />
    </>
  );
};

export default Navigation; 
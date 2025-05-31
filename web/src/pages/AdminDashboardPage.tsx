import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { useTranslation } from 'react-i18next';
import {
  Box,
  Typography,
  Card,
  CardContent,
  Button,
  Skeleton,
  Snackbar,
  Alert,
  Container,
  Stack,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  TablePagination,
  IconButton,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
} from '@mui/material';
import {
  People as PeopleIcon,
  Backup as BackupIcon,
  ImportExport as ImportExportIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  Add as AddIcon,
} from '@mui/icons-material';
import Navigation from '../components/Navigation';
import { API_CONFIG } from '../config/api';

interface User {
  id: string;
  email: string;
}

interface UserListResponse {
  users: User[];
  total_count: number;
  page: number;
  limit: number;
  total_pages: number;
}

const AdminDashboardPage: React.FC = () => {
  const { t } = useTranslation(['admin']);
  const navigate = useNavigate();
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [userCount, setUserCount] = useState(0);
  const [users, setUsers] = useState<User[]>([]);
  const [page, setPage] = useState(0);
  const [limit, setLimit] = useState(25);
  const [totalPages, setTotalPages] = useState(0);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [userToDelete, setUserToDelete] = useState<User | null>(null);
  const [editDialogOpen, setEditDialogOpen] = useState(false);
  const [userToEdit, setUserToEdit] = useState<User | null>(null);
  const [editEmail, setEditEmail] = useState('');
  const [addDialogOpen, setAddDialogOpen] = useState(false);
  const [newUserData, setNewUserData] = useState({
    email: '',
    password: '',
  });

  const loadUsers = async (pageNum: number, pageSize: number) => {
    try {
      setLoading(true);
      setError(null);

      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/users/list?page=${pageNum + 1}&limit=${pageSize}`,
        {
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
        }
      );

      if (!response.ok) {
        throw new Error('Failed to load users');
      }

      const data: UserListResponse = await response.json();
      setUsers(data.users);
      setUserCount(data.total_count);
      setTotalPages(data.total_pages);
    } catch (err) {
      console.error('Failed to load users:', err);
      setError(t('errors.loadFailed'));
    } finally {
      setLoading(false);
    }
  };

  const handleDeleteUser = async () => {
    if (!userToDelete) return;

    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/users?id=${userToDelete.id}`,
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
        throw new Error('Failed to delete user');
      }

      // Refresh the user list
      loadUsers(page, limit);
      setDeleteDialogOpen(false);
      setUserToDelete(null);
    } catch (err) {
      console.error('Failed to delete user:', err);
      setError(t('errors.deleteFailed'));
    }
  };

  const handleEditUser = async () => {
    if (!userToEdit) return;

    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/users?id=${userToEdit.id}`,
        {
          method: 'PATCH',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
          body: JSON.stringify({
            email: editEmail,
          }),
        }
      );

      if (!response.ok) {
        throw new Error('Failed to update user');
      }

      // Refresh the user list
      loadUsers(page, limit);
      setEditDialogOpen(false);
      setUserToEdit(null);
      setEditEmail('');
    } catch (err) {
      console.error('Failed to update user:', err);
      setError(t('errors.updateFailed'));
    }
  };

  const handleAddUser = async () => {
    try {
      const response = await fetch(
        `${API_CONFIG.BASE_URL}/api/users/register`,
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
            'ngrok-skip-browser-warning': 'true',
          },
          credentials: 'include',
          body: JSON.stringify(newUserData),
        }
      );

      if (!response.ok) {
        throw new Error('Failed to create user');
      }

      // Refresh the user list
      loadUsers(page, limit);
      setAddDialogOpen(false);
      setNewUserData({ email: '', password: '' });
    } catch (err) {
      console.error('Failed to create user:', err);
      setError(t('errors.createFailed'));
    }
  };

  useEffect(() => {
    loadUsers(page, limit);
  }, [page, limit]);

  const handleChangePage = (event: unknown, newPage: number) => {
    setPage(newPage);
  };

  const handleChangeRowsPerPage = (event: React.ChangeEvent<HTMLInputElement>) => {
    setLimit(parseInt(event.target.value, 10));
    setPage(0);
  };

  const handleEditClick = (user: User) => {
    setUserToEdit(user);
    setEditEmail(user.email);
    setEditDialogOpen(true);
  };

  const handleDeleteClick = (user: User) => {
    setUserToDelete(user);
    setDeleteDialogOpen(true);
  };

  return (
    <Box sx={{ display: 'flex' }}>
      <Navigation isAdmin={true} />
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          p: 3,
          backgroundColor: '#F1F1F1',
          minHeight: '100vh',
        }}
      >
        <Container maxWidth="lg">
          {/* Header */}
          <Typography
            variant="h1"
            sx={{
              fontSize: '24px',
              fontWeight: 700,
              fontFamily: 'Roboto',
              mb: 4,
              textAlign: 'center',
            }}
          >
            {t('title')}
          </Typography>

          {/* Metrics Card */}
          <Card sx={{ mb: 4 }}>
            <CardContent>
              {loading ? (
                <Box sx={{ display: 'flex', alignItems: 'center', gap: 2 }}>
                  <Skeleton variant="circular" width={40} height={40} />
                  <Box sx={{ flex: 1 }}>
                    <Skeleton variant="text" width="60%" height={32} />
                    <Skeleton variant="text" width="40%" height={24} />
                  </Box>
                </Box>
              ) : (
                <Box sx={{ display: 'flex', alignItems: 'center', gap: 2 }}>
                  <PeopleIcon sx={{ fontSize: 40, color: '#2196F3' }} />
                  <Box>
                    <Typography variant="h6" gutterBottom>
                      {t('metrics.userCount')}
                    </Typography>
                    <Typography variant="h4" color="text.secondary">
                      {userCount}
                    </Typography>
                  </Box>
                </Box>
              )}
            </CardContent>
          </Card>

          {/* Action Buttons */}
          <Stack
            direction={{ xs: 'column', sm: 'row' }}
            spacing={2}
            sx={{ mb: 4 }}
            useFlexGap
            flexWrap="wrap"
            justifyContent="center"
          >
            <Button
              fullWidth
              variant="contained"
              startIcon={<AddIcon />}
              onClick={() => setAddDialogOpen(true)}
              sx={{ backgroundColor: '#4CAF50', flex: { sm: '1 1 calc(33% - 8px)', md: '1 1 calc(25% - 12px)' } }}
            >
              {t('buttons.addUser')}
            </Button>
            <Button
              fullWidth
              variant="contained"
              startIcon={<BackupIcon />}
              onClick={() => navigate('/admin/backups')}
              sx={{ backgroundColor: '#2196F3', flex: { sm: '1 1 calc(33% - 8px)', md: '1 1 calc(25% - 12px)' } }}
            >
              {t('buttons.backups')}
            </Button>
            <Button
              fullWidth
              variant="contained"
              startIcon={<ImportExportIcon />}
              onClick={() => navigate('/admin/import-export')}
              sx={{ backgroundColor: '#4CAF50', flex: { sm: '1 1 calc(33% - 8px)', md: '1 1 calc(25% - 12px)' } }}
            >
              {t('buttons.importExport')}
            </Button>
          </Stack>

          {/* Users Table */}
          <TableContainer component={Paper} sx={{ mb: 4 }}>
            <Table>
              <TableHead>
                <TableRow>
                  <TableCell>{t('table.id')}</TableCell>
                  <TableCell>{t('table.email')}</TableCell>
                  <TableCell align="right">{t('table.actions')}</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {loading ? (
                  Array.from(new Array(limit)).map((_, index) => (
                    <TableRow key={index}>
                      <TableCell><Skeleton /></TableCell>
                      <TableCell><Skeleton /></TableCell>
                      <TableCell><Skeleton /></TableCell>
                    </TableRow>
                  ))
                ) : (
                  users.map((user) => (
                    <TableRow key={user.id}>
                      <TableCell>{user.id}</TableCell>
                      <TableCell>{user.email}</TableCell>
                      <TableCell align="right">
                        <IconButton onClick={() => handleEditClick(user)}>
                          <EditIcon />
                        </IconButton>
                        <IconButton color="error" onClick={() => handleDeleteClick(user)}>
                          <DeleteIcon />
                        </IconButton>
                      </TableCell>
                    </TableRow>
                  ))
                )}
              </TableBody>
            </Table>
            <TablePagination
              component="div"
              count={userCount}
              page={page}
              onPageChange={handleChangePage}
              rowsPerPage={limit}
              onRowsPerPageChange={handleChangeRowsPerPage}
              rowsPerPageOptions={[10, 25, 50]}
            />
          </TableContainer>
        </Container>
      </Box>

      {/* Edit Dialog */}
      <Dialog
        open={editDialogOpen}
        onClose={() => setEditDialogOpen(false)}
      >
        <DialogTitle>{t('editDialog.title')}</DialogTitle>
        <DialogContent>
          <Box sx={{ pt: 2, width: 400 }}>
            <TextField
              fullWidth
              label={t('editDialog.email')}
              value={editEmail}
              onChange={(e) => setEditEmail(e.target.value)}
            />
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setEditDialogOpen(false)} color="inherit">
            {t('editDialog.cancel')}
          </Button>
          <Button onClick={handleEditUser} variant="contained" color="primary">
            {t('editDialog.save')}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Delete Confirmation Dialog */}
      <Dialog
        open={deleteDialogOpen}
        onClose={() => setDeleteDialogOpen(false)}
      >
        <DialogTitle>{t('deleteDialog.title')}</DialogTitle>
        <DialogContent>
          <Typography>
            {t('deleteDialog.message', { email: userToDelete?.email })}
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDeleteDialogOpen(false)} color="inherit">
            {t('deleteDialog.cancel')}
          </Button>
          <Button onClick={handleDeleteUser} color="error" variant="contained">
            {t('deleteDialog.confirm')}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Add User Dialog */}
      <Dialog
        open={addDialogOpen}
        onClose={() => setAddDialogOpen(false)}
      >
        <DialogTitle>{t('addDialog.title')}</DialogTitle>
        <DialogContent>
          <Box sx={{ pt: 2, width: 400 }}>
            <TextField
              fullWidth
              label={t('addDialog.email')}
              value={newUserData.email}
              onChange={(e) => setNewUserData({ ...newUserData, email: e.target.value })}
              sx={{ mb: 2 }}
            />
            <TextField
              fullWidth
              type="password"
              label={t('addDialog.password')}
              value={newUserData.password}
              onChange={(e) => setNewUserData({ ...newUserData, password: e.target.value })}
            />
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setAddDialogOpen(false)} color="inherit">
            {t('addDialog.cancel')}
          </Button>
          <Button onClick={handleAddUser} variant="contained" color="primary">
            {t('addDialog.save')}
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

export default AdminDashboardPage; 
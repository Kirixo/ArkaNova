import dashboard from '../../locales/uk/dashboard.json';
import sensors from '../../locales/uk/sensors.json';
import panels from '../../locales/uk/panels.json';
import profile from '../../locales/uk/profile.json';
import admin from '../locales/uk-UA/admin.json';

export const ukUA = {
  dashboard,
  sensors,
  panels,
  profile,
  admin,
  translation: {
    login: {
      title: 'Вхід',
      email: 'Електронна пошта',
      password: 'Пароль',
      loginButton: 'Увійти',
      register: 'Реєстрація',
      forgotPassword: 'Забули пароль',
      invalidCredentials: 'Невірні облікові дані',
      noInternet: 'Немає підключення до Інтернету',
      featureComingSoon: 'Функція скоро з\'явиться',
      emailPlaceholder: 'Введіть електронну пошту',
      passwordPlaceholder: 'Введіть пароль',
      invalidEmail: 'Невірний формат електронної пошти'
    },
    register: {
      title: 'Створити обліковий запис',
      email: 'Електронна пошта',
      password: 'Пароль',
      confirmPassword: 'Підтвердіть пароль',
      registerButton: 'Зареєструватися',
      backToLogin: 'Повернутися до входу',
      emailExists: 'Така електронна пошта вже існує',
      noInternet: 'Немає підключення до Інтернету',
      passwordTooShort: 'Пароль має містити щонайменше 8 символів',
      passwordsDontMatch: 'Паролі не співпадають',
      invalidEmail: 'Невірний формат електронної пошти',
      adminEmailRestricted: 'Реєстрація з поштою @arkanova.com обмежена',
      success: 'Реєстрація успішна'
    },
    home: {
      greeting: 'Вітаємо, {{email}}',
      temperature: {
        title: 'Температура',
        status: {
          normal: 'Нормальна',
          warning: 'Увага',
          error: 'Критична'
        },
        lastUpdated: 'Останнє оновлення: {{time}}'
      },
      energy: {
        title: 'Виробництво енергії',
        unit: 'кВт·год',
        lastUpdated: 'Останнє оновлення: {{time}}'
      },
      buttons: {
        viewDashboard: 'Переглянути панель',
        viewSensors: 'Переглянути сенсори'
      },
      errors: {
        failedMetrics: 'Не вдалося завантажити метрики',
        noPanels: 'Сонячні панелі не знайдено'
      }
    },
    navigation: {
      home: 'Головна',
      dashboard: 'Панель',
      sensors: 'Сенсори',
      solarPanels: 'Сонячні панелі',
      profile: 'Профіль',
      logout: 'Вийти',
      userManagement: 'Керування користувачами'
    },
    common: {
      loading: 'Завантаження...',
      error: 'Помилка',
      success: 'Успішно'
    }
  }
}; 
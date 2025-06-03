from locust import HttpUser, task, between

class MyApiUser(HttpUser):
    wait_time = between(0.5, 1.5)

    @task # Позначає метод як завдання для віртуального користувача
    def get_my_endpoint(self):
        self.client.get("/api/users?id=12")

    def on_start(self):
        pass
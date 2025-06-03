# Backend part of ArkaNova
*Before starting backend API you must be sure that database works properly and ready to get connections.*
For Kubernetes deployment use: 
```bash
docker build -t kirixo/arkanova-api:latest .
kubectl apply -f api-deployment.yaml
kubectl apply -f api-service.yaml
```
If you wanna set the amount of replicas use command
```bash
kubectl scale deployment my-qt-api-deployment --replicas=2 # Desired amount
```

For testing:
If you don't have Locust, run:
```bash
pip install locust
```
then you can start testing:
```bash
locust -f locustfile.py --host=http://localhost:30000 # Your actual port
```

For checking your actual port use:
```bash
kubectl get services
```
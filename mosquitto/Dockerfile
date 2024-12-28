FROM eclipse-mosquitto:latest

COPY mosquitto.conf /mosquitto/config/mosquitto.conf
COPY pwfile /mosquitto/config/pwfile

WORKDIR /mosquitto/config
RUN chmod 644 mosquitto.conf pwfile

CMD ["mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]

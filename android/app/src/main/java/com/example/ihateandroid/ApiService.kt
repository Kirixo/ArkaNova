package com.example.ihateandroid

import retrofit2.Response
import retrofit2.http.*
import com.google.gson.annotations.SerializedName

interface ApiService {
    @POST("/api/users/login")
    suspend fun login(@Body request: LoginRequest): Response<UserResponse>

    @POST("/api/users/register")
    suspend fun register(@Body request: RegisterRequest): Response<UserResponse>

    @GET("/api/users")
    suspend fun getUserDetails(
        @Query("id") userId: Int
    ): Response<UserResponse>

    @PATCH("/api/users")
    suspend fun updateUser(
        @Query("id") userId: Int,
        @Body request: UpdateUserRequest
    ): Response<Unit>

    @DELETE("/api/users")
    suspend fun deleteUser(
        @Query("id") userId: Int
    ): Response<Unit>

    // Solar Panel Endpoints
    @GET("/api/solarpanel/list/user")
    suspend fun getUserPanels(
        @Query("user_id") userId: Int,
        @Query("page") page: Int,
        @Query("limit") limit: Int
    ): Response<PanelListResponse>

    @POST("/api/solarpanel")
    suspend fun createPanel(
        @Body request: CreatePanelRequest
    ): Response<SolarPanel>

    @PATCH("/api/solarpanel") // New endpoint for updating panel
    suspend fun updateSolarPanel(
        @Query("id") panelId: Int,
        @Body request: UpdatePanelRequest
    ): Response<SolarPanel> // Assuming API returns the updated panel

    @DELETE("/api/solarpanel")
    suspend fun deleteSolarPanel(@Query("id") panelId: Int): Response<Unit>


    // Sensor Endpoints
    @GET("/api/sensor/list/solarpanel")
    suspend fun getPanelSensors(
        @Query("panel_id") panelId: Int,
        @Query("page") page: Int,
        @Query("limit") limit: Int
    ): Response<SensorListResponse>

    @POST("/api/sensor")
    suspend fun createSensor(
        @Body request: CreateSensorRequest
    ): Response<Sensor>

    @DELETE("/api/sensor")
    suspend fun deleteSensor(@Query("id") sensorId: Int): Response<Unit>


    // Measurement Endpoints
    @GET("/api/measurement/list/sensor")
    suspend fun getSensorMeasurements(
        @Query("sensor_id") sensorId: Int,
        @Query("start_date") startDate: String,
        @Query("end_date") endDate: String
    ): Response<MeasurementListResponse>
}

// Data classes
data class LoginRequest(val email: String, val password: String)
data class RegisterRequest(val email: String, val password: String)
data class UserResponse(val id: Int, val email: String)

data class UpdateUserRequest(
    val email: String,
    val password: String? = null
)

data class PanelListResponse(
    val panels: List<SolarPanel>,
    val total_count: Int
)

data class SolarPanel(
    val id: Int,
    var location: String, // Made var to allow modification after update
    @SerializedName("user_id") val userId: Int,
    @SerializedName("created_at") val createdAt: String? = null,
    @SerializedName("updated_at") val updatedAt: String? = null
)

data class UpdatePanelRequest( // New data class for updating panel
    val location: String
)

data class SensorListResponse(
    val sensors: List<Sensor>,
    val total_count: Int
)

data class Sensor(
    val id: Int,
    val type: SensorType,
    @SerializedName("solar_panel_id") val solarPanelId: Int,
    @SerializedName("created_at") val createdAt: String? = null,
    @SerializedName("updated_at") val updatedAt: String? = null
)

data class SensorType(
    val id: Int,
    val name: String
)

data class MeasurementListResponse(
    val measurements: List<Measurement>,
    val total_count: Int
)

data class Measurement(
    val id: Int,
    val data: Float,
    @SerializedName("recorded_at") val recordedAt: Long,
    @SerializedName("sensor_id") val sensorId: Int
)

data class CreatePanelRequest(
    val location: String,
    @SerializedName("user_id") val userId: Int
)

data class CreateSensorRequest(
    @SerializedName("type_id") val typeId: Int,
    @SerializedName("solar_panel_id") val solarPanelId: Int
)

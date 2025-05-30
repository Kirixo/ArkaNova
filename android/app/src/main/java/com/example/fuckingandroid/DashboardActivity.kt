package com.example.fuckingandroid

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.view.animation.AnimationUtils
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.ImageView
import android.widget.Spinner
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import com.github.mikephil.charting.charts.CombinedChart
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.CombinedData
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import com.github.mikephil.charting.formatter.ValueFormatter
import com.google.android.material.bottomnavigation.BottomNavigationView
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import retrofit2.Response
import java.text.SimpleDateFormat
import java.util.*
// Ensure LoggerUtil is correctly imported if it's in a different package
// import com.example.fuckingandroid.util.LoggerUtil;


class DashboardActivity : AppCompatActivity() {
    private val scope = CoroutineScope(Dispatchers.Main)
    private var panels: List<SolarPanel> = emptyList()
    private var sensors: List<Sensor> = emptyList()
    private var selectedPanelIdFromIntent: Int? = null // To store ID from HomeActivity
    private var selectedPanelId: Int? = null
    private var selectedSensorId: Int? = null
    private var selectedTimeRange: String = "Daily"

    private lateinit var panelSpinner: Spinner // Declare here for broader access

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_dashboard)

        selectedPanelIdFromIntent = intent.getIntExtra("SELECTED_PANEL_ID", -1)
            .takeIf { it != -1 }


        // Initialize UI components
        val dashboardHeader = findViewById<TextView>(R.id.dashboard_header)
        panelSpinner = findViewById(R.id.panel_spinner) // Initialize here
        val sensorSpinner = findViewById<Spinner>(R.id.sensor_spinner)
        val timeRangeSpinner = findViewById<Spinner>(R.id.time_range_spinner)
        val dataCard = findViewById<CardView>(R.id.data_card)
        val dataTitle = findViewById<TextView>(R.id.data_title)
        val dataIcon = findViewById<ImageView>(R.id.data_icon)
        val dataChart = findViewById<CombinedChart>(R.id.data_chart)
        val summaryText = findViewById<TextView>(R.id.summary_text)
        val bottomNav = findViewById<BottomNavigationView>(R.id.bottom_nav)

        // Apply fade-in animation
        val fadeIn = AnimationUtils.loadAnimation(this, R.anim.fade_in)
        dataCard.startAnimation(fadeIn)

        // Initialize time range spinner
        val timeRanges = arrayOf("Hourly", "Daily", "Weekly")
        timeRangeSpinner.adapter = ArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item, timeRanges)
        timeRangeSpinner.setSelection(1) // Default to Daily

        // Get userId
        val prefs = getSharedPreferences("ArkaNovaPrefs", MODE_PRIVATE)
        val userId = prefs.getInt("userId", 0)
        if (userId == 0) {
            LoggerUtil.logWarning("No userId found in SharedPreferences")
            startActivity(Intent(this, MainActivity::class.java))
            finish()
            return
        }

        // Bottom navigation
        bottomNav.selectedItemId = R.id.nav_dashboard
        bottomNav.setOnItemSelectedListener { item ->
            when (item.itemId) {
                R.id.nav_home -> {
                    startActivity(Intent(this, HomeActivity::class.java))
                    finish()
                    true
                }
                R.id.nav_dashboard -> true
                R.id.nav_profile -> {
                    startActivity(Intent(this, ProfileActivity::class.java))
                    true
                }
                else -> {
                    LoggerUtil.logWarning("Unknown nav item selected: ${item.itemId}")
                    false
                }
            }
        }

        // Fetch panels and initialize
        scope.launch {
            fetchPanels(userId, panelSpinner, sensorSpinner, dataTitle, dataIcon, dataChart, summaryText)
        }

        // Filter listeners
        panelSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, position: Int, id: Long) {
                selectedPanelId = panels.getOrNull(position)?.id
                selectedPanelId?.let { panelId ->
                    // Avoid re-fetching if this selection was due to programmatic set from intent
                    if (selectedPanelIdFromIntent != panelId || parent.tag != "INIT_FROM_INTENT") {
                        scope.launch {
                            fetchSensors(panelId, sensorSpinner, dataTitle, dataIcon, dataChart, summaryText)
                        }
                    }
                    parent.tag = null // Reset tag after initial handling
                }
            }
            override fun onNothingSelected(parent: AdapterView<*>) {}
        }

        sensorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, position: Int, id: Long) {
                selectedSensorId = sensors.getOrNull(position)?.id
                selectedSensorId?.let { sensorId ->
                    scope.launch {
                        fetchMeasurements(sensorId, selectedTimeRange, dataTitle, dataIcon, dataChart, summaryText)
                    }
                }
            }
            override fun onNothingSelected(parent: AdapterView<*>) {}
        }

        timeRangeSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, position: Int, id: Long) {
                selectedTimeRange = timeRanges[position]
                selectedSensorId?.let { sensorId ->
                    scope.launch {
                        fetchMeasurements(sensorId, selectedTimeRange, dataTitle, dataIcon, dataChart, summaryText)
                    }
                }
            }
            override fun onNothingSelected(parent: AdapterView<*>) {}
        }

        // Configure chart
        configureChart(dataChart)
    }

    private fun configureChart(chart: CombinedChart) {
        chart.description.isEnabled = false
        chart.setTouchEnabled(true)
        chart.isDragEnabled = true
        chart.setPinchZoom(true)
        chart.xAxis.position = XAxis.XAxisPosition.BOTTOM
        chart.xAxis.setDrawGridLines(true)
        chart.axisLeft.setDrawGridLines(true)
        chart.axisRight.isEnabled = false
        chart.xAxis.setAvoidFirstLastClipping(true)
        chart.setDrawBorders(true)
        chart.setBorderColor(getColor(R.color.text_gray_dark))
        chart.animateXY(300, 300)
        chart.legend.textColor = getColor(R.color.text_gray)
        chart.axisLeft.textColor = getColor(R.color.text_gray)
        chart.xAxis.textColor = getColor(R.color.text_gray)
        chart.axisLeft.axisMinimum = 0f
        chart.setDrawOrder(arrayOf(CombinedChart.DrawOrder.LINE))
    }

    private suspend fun fetchPanels(
        userId: Int,
        panelSpinner: Spinner,
        sensorSpinner: Spinner,
        dataTitle: TextView,
        dataIcon: ImageView,
        dataChart: CombinedChart,
        summaryText: TextView
    ) {
        withContext(Dispatchers.IO) {
            try {
                LoggerUtil.logInfo("Fetching panels for userId=$userId")
                val response: Response<PanelListResponse> = RetrofitClient.apiService.getUserPanels(userId, page = 1, limit = 100) // Fetch more
                LoggerUtil.logInfo("Panel response: code=${response.code()}, body=${response.body()?.panels?.size ?: 0} panels")

                if (!response.isSuccessful) {
                    val errorBody = response.errorBody()?.string()
                    LoggerUtil.logError("Panel fetch failed: code=${response.code()}, error=$errorBody")
                    withContext(Dispatchers.Main) {
                        handleApiError(response.code())
                    }
                    return@withContext
                }

                panels = response.body()?.panels ?: emptyList()
                if (panels.isEmpty()) {
                    LoggerUtil.logWarning("No panels found for userId=$userId")
                    withContext(Dispatchers.Main) {
                        Toast.makeText(this@DashboardActivity, "No panels registered. Add one via Home screen.", Toast.LENGTH_LONG).show()
                        panelSpinner.adapter = ArrayAdapter<String>(this@DashboardActivity, android.R.layout.simple_spinner_dropdown_item, listOf("No panels"))
                        panelSpinner.isEnabled = false
                        sensorSpinner.isEnabled = false
                        dataChart.clear()
                        dataTitle.text = getString(R.string.no_panels_available)
                        summaryText.text = getString(R.string.please_add_a_solar_panel)
                    }
                    return@withContext
                }

                withContext(Dispatchers.Main) {
                    panelSpinner.adapter = ArrayAdapter(
                        this@DashboardActivity,
                        android.R.layout.simple_spinner_dropdown_item,
                        panels.map { "Panel ID: ${it.id} (${it.location})" }
                    )
                    panelSpinner.isEnabled = true

                    // Pre-select panel if ID came from intent
                    selectedPanelIdFromIntent?.let { intentPanelId ->
                        val position = panels.indexOfFirst { it.id == intentPanelId }
                        if (position != -1) {
                            panelSpinner.tag = "INIT_FROM_INTENT" // Mark that this is an initial set
                            panelSpinner.setSelection(position)
                            // Fetch sensors for this pre-selected panel directly
                            // The onItemSelected listener will handle other cases
                            selectedPanelId = panels[position].id // Update selectedPanelId
                            fetchSensors(panels[position].id, sensorSpinner, dataTitle, dataIcon, dataChart, summaryText)
                        } else {
                            // If panel from intent not found, default to first panel
                            if (panels.isNotEmpty()) {
                                panelSpinner.setSelection(0)
                                selectedPanelId = panels[0].id
                                fetchSensors(panels[0].id, sensorSpinner, dataTitle, dataIcon, dataChart, summaryText)
                            }
                        }
                    } ?: run {
                        // Default to first panel if no intent ID
                        if (panels.isNotEmpty()) {
                            panelSpinner.setSelection(0) // This will trigger onItemSelected
                            selectedPanelId = panels[0].id
                            // fetchSensors will be called by onItemSelected listener
                        }
                    }
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Unexpected error in fetchPanels", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(this@DashboardActivity, "Unexpected error: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }
        }
    }

    // fetchSensors, fetchMeasurements, formatTimestamp, handleApiError remain largely the same
    // Ensure they use the class-level selectedPanelId and selectedSensorId

    private suspend fun fetchSensors(
        panelId: Int,
        sensorSpinner: Spinner,
        dataTitle: TextView,
        dataIcon: ImageView,
        dataChart: CombinedChart,
        summaryText: TextView
    ) {
        withContext(Dispatchers.IO) {
            try {
                LoggerUtil.logInfo("Fetching sensors for panelId=$panelId")
                val response: Response<SensorListResponse> = RetrofitClient.apiService.getPanelSensors(panelId, page = 1, limit = 25)
                LoggerUtil.logInfo("Sensor response: code=${response.code()}, body=${response.body()?.sensors?.size ?: 0} sensors")

                if (!response.isSuccessful) {
                    val errorBody = response.errorBody()?.string()
                    LoggerUtil.logError("Sensor fetch failed: code=${response.code()}, error=$errorBody")
                    withContext(Dispatchers.Main) {
                        handleApiError(response.code())
                    }
                    return@withContext
                }

                sensors = response.body()?.sensors ?: emptyList()
                withContext(Dispatchers.Main) {
                    if (sensors.isEmpty()) {
                        LoggerUtil.logWarning("No sensors found for panelId=$panelId")
                        Toast.makeText(this@DashboardActivity, "No sensors found for this panel.", Toast.LENGTH_LONG).show()
                        sensorSpinner.adapter = ArrayAdapter<String>(this@DashboardActivity, android.R.layout.simple_spinner_dropdown_item, listOf("No sensors available"))
                        sensorSpinner.isEnabled = false
                        dataChart.clear()
                        dataTitle.text = getString(R.string.no_sensors_title)
                        dataIcon.setImageResource(R.drawable.ic_no_data)
                        summaryText.text = getString(R.string.please_add_sensors_to_panel)
                        selectedSensorId = null
                        return@withContext
                    }

                    sensorSpinner.adapter = ArrayAdapter(
                        this@DashboardActivity,
                        android.R.layout.simple_spinner_dropdown_item,
                        sensors.map { "${it.type.name.replaceFirstChar { if (it.isLowerCase()) it.titlecase() else it.toString() }} Sensor (ID: ${it.id})" }
                    )
                    sensorSpinner.isEnabled = true
                    if (sensors.isNotEmpty()) {
                        sensorSpinner.setSelection(0) // This will trigger its onItemSelected
                        selectedSensorId = sensors[0].id
                    }
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Unexpected error in fetchSensors", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(this@DashboardActivity, "Unexpected error: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }
        }
    }

    private suspend fun fetchMeasurements(
        sensorId: Int,
        timeRange: String,
        dataTitle: TextView,
        dataIcon: ImageView,
        dataChart: CombinedChart,
        summaryText: TextView
    ) {
        withContext(Dispatchers.IO) {
            try {
                LoggerUtil.logInfo("Fetching measurements for sensorId=$sensorId, timeRange=$timeRange")
                val calendar = Calendar.getInstance()
                val endDate = SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'", Locale.getDefault()).format(Date())

                val tempCalendar = Calendar.getInstance()
                when (timeRange) {
                    "Hourly" -> tempCalendar.add(Calendar.HOUR_OF_DAY, -1)
                    "Daily" -> tempCalendar.add(Calendar.DAY_OF_YEAR, -1)
                    "Weekly" -> tempCalendar.add(Calendar.WEEK_OF_YEAR, -1)
                }
                val startDate = SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'", Locale.getDefault()).format(tempCalendar.time)


                val response: Response<MeasurementListResponse> = RetrofitClient.apiService.getSensorMeasurements(sensorId, startDate, endDate)
                LoggerUtil.logInfo("Measurement response: code=${response.code()}, body=${response.body()?.measurements?.size ?: 0} measurements")

                if (!response.isSuccessful) {
                    val errorBody = response.errorBody()?.string()
                    LoggerUtil.logError("Measurement fetch failed: code=${response.code()}, error=$errorBody")
                    withContext(Dispatchers.Main) {
                        handleApiError(response.code())
                    }
                    return@withContext
                }

                val measurements = response.body()?.measurements?.sortedBy { it.recordedAt } ?: emptyList()

                withContext(Dispatchers.Main) {
                    if (measurements.isEmpty()) {
                        LoggerUtil.logWarning("No measurements found for sensorId=$sensorId for $timeRange")
                        Toast.makeText(this@DashboardActivity, "No measurements for selected period.", Toast.LENGTH_LONG).show()
                        dataChart.clear()
                        dataChart.invalidate()
                        dataTitle.text = getString(R.string.no_data_available)
                        summaryText.text = getString(R.string.na_for_period)
                        return@withContext
                    }

                    val sensor = sensors.find { it.id == sensorId }
                    if (sensor == null) {
                        LoggerUtil.logWarning("Sensor not found for sensorId=$sensorId during measurement processing")
                        dataChart.clear()
                        dataChart.invalidate()
                        dataTitle.text = getString(R.string.sensor_error_title)
                        summaryText.text = getString(R.string.could_not_load_sensor_details)
                        return@withContext
                    }

                    LoggerUtil.logInfo("Measurements count: ${measurements.size}")
                    measurements.forEach { m ->
                        LoggerUtil.logInfo("Measurement: recorded_at=${m.recordedAt}, data=${m.data}, formatted=${formatTimestamp(m.recordedAt, timeRange)}")
                    }

                    dataChart.clear()
                    val combinedData = CombinedData()
                    val baseTime = measurements.firstOrNull()?.recordedAt ?: System.currentTimeMillis()


                    when (sensor.type.name.lowercase(Locale.ROOT)) {
                        "temperature" -> {
                            dataTitle.text = getString(R.string.temperature_label_chart)
                            dataIcon.setImageResource(R.drawable.ic_thermometer)
                            val entries = measurements.map { m ->
                                Entry(((m.recordedAt - baseTime) / 1000f), m.data.toFloat())
                            }
                            val dataSet = LineDataSet(entries, getString(R.string.temperature_series_label))

                            val lastTemp = entries.lastOrNull()?.y ?: 0f
                            dataSet.color = when {
                                lastTemp < 50 -> getColor(R.color.primary_green)
                                lastTemp <= 60 -> getColor(R.color.warning_orange)
                                else -> getColor(R.color.critical_red)
                            }
                            dataSet.setDrawFilled(true)
                            dataSet.fillColor = dataSet.color
                            dataSet.setDrawCircles(true)
                            dataSet.circleRadius = 3f
                            dataSet.circleHoleColor = dataSet.color
                            dataSet.lineWidth = 2f
                            dataSet.setDrawValues(true)
                            dataSet.valueTextColor = getColor(R.color.text_gray)
                            combinedData.setData(LineData(dataSet))

                            val avgTemp = measurements.map { it.data.toFloat() }.average().toFloat()
                            summaryText.text = getString(R.string.avg_temp_summary, String.format(Locale.US, "%.2f", avgTemp))
                        }
                        "power" -> {
                            dataTitle.text = getString(R.string.energy_production_label_chart)
                            dataIcon.setImageResource(R.drawable.ic_lightning)
                            val entries = measurements.map { m ->
                                Entry(((m.recordedAt - baseTime) / 1000f), m.data.toFloat())
                            }
                            val dataSet = LineDataSet(entries, getString(R.string.energy_series_label))
                            dataSet.color = getColor(R.color.primary_green)
                            dataSet.setDrawFilled(true)
                            dataSet.fillColor = dataSet.color
                            dataSet.setDrawCircles(true)
                            dataSet.circleRadius = 3f
                            dataSet.circleHoleColor = dataSet.color;
                            dataSet.lineWidth = 2f
                            dataSet.setDrawValues(true)
                            dataSet.valueTextColor = getColor(R.color.text_gray)
                            combinedData.setData(LineData(dataSet))

                            val avgEnergy = measurements.map { it.data.toFloat() }.average().toFloat()
                            summaryText.text = getString(R.string.avg_energy_summary, String.format(Locale.US, "%.2f", avgEnergy))
                        }
                        else -> {
                            LoggerUtil.logWarning("Unknown sensor type: ${sensor.type.name}")
                            dataChart.clear()
                            dataTitle.text = getString(R.string.unknown_data_type_label)
                            dataIcon.setImageResource(R.drawable.ic_no_data)
                            summaryText.text = getString(R.string.na_generic)
                            return@withContext
                        }
                    }

                    dataChart.xAxis.valueFormatter = object : ValueFormatter() {
                        override fun getFormattedValue(value: Float): String {
                            return formatTimestamp((value * 1000f).toLong() + baseTime, timeRange)
                        }
                    }

                    val secondsInHour = 3600f
                    val secondsInDay = 86400f
                    val secondsInWeek = 604800f

                    dataChart.xAxis.isGranularityEnabled = true

                    dataChart.setVisibleXRangeMaximum(when (timeRange) {
                        "Hourly" -> secondsInHour
                        "Daily" -> secondsInDay
                        "Weekly" -> secondsInWeek
                        else -> secondsInDay
                    })
                    dataChart.xAxis.setAvoidFirstLastClipping(true)

                    dataChart.data = combinedData
                    dataChart.fitScreen()
                    dataChart.moveViewToX(0f)
                    dataChart.notifyDataSetChanged()
                    dataChart.invalidate()
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Unexpected error in fetchMeasurements", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(this@DashboardActivity, "Unexpected error fetching measurements: ${e.message}", Toast.LENGTH_LONG).show()
                    dataChart.clear()
                    dataChart.invalidate()
                    dataTitle.text = getString(R.string.error_title)
                    summaryText.text = getString(R.string.could_not_load_data)
                }
            }
        }
    }

    private fun formatTimestamp(timestampMillis: Long, timeRange: String): String {
        return try {
            val outputFormat = when (timeRange) {
                "Hourly" -> SimpleDateFormat("HH:mm:ss", Locale.getDefault())
                "Daily" -> SimpleDateFormat("MMM dd, HH:mm", Locale.getDefault())
                "Weekly" -> SimpleDateFormat("MMM dd", Locale.getDefault())
                else -> SimpleDateFormat("MMM dd, hh:mm a", Locale.getDefault())
            }
            val utcCalendar = Calendar.getInstance(TimeZone.getTimeZone("UTC"))
            utcCalendar.timeInMillis = timestampMillis
            outputFormat.timeZone = TimeZone.getDefault()
            outputFormat.format(utcCalendar.time)
        } catch (e: Exception) {
            LoggerUtil.logError("Failed to format timestamp: $timestampMillis", e)
            timestampMillis.toString()
        }
    }

    private fun handleApiError(code: Int) {
        when (code) {
            401, 403 -> {
                LoggerUtil.logWarning("API Error $code: Session expired or forbidden. Logging out.")
                getSharedPreferences("ArkaNovaPrefs", MODE_PRIVATE).edit().clear().apply()
                val intent = Intent(this@DashboardActivity, MainActivity::class.java)
                intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                startActivity(intent)
                finish()
                Toast.makeText(this@DashboardActivity, "Session expired. Please log in again.", Toast.LENGTH_LONG).show()
            }
            else -> {
                LoggerUtil.logError("API error: code=$code")
                Toast.makeText(this@DashboardActivity, "Failed to load data (Error: $code). Try again.", Toast.LENGTH_LONG).show()
            }
        }
    }
}

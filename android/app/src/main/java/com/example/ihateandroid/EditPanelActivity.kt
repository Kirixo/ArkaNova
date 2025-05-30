package com.example.ihateandroid

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.MenuItem
import android.view.View
import android.widget.ArrayAdapter
import android.widget.ProgressBar
import android.widget.Spinner
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.button.MaterialButton
import com.google.android.material.textfield.TextInputEditText
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import retrofit2.Response

class EditPanelActivity : AppCompatActivity() {

    private val scope = CoroutineScope(Dispatchers.Main)
    private var panelId: Int = -1
    private lateinit var currentPanelLocation: String // Store current location

    private lateinit var toolbar: MaterialToolbar
    private lateinit var panelNameHeaderTextView: TextView // For displaying "Edit Panel: [Name]"
    private lateinit var panelNameEditText: TextInputEditText
    private lateinit var savePanelNameButton: MaterialButton
    private lateinit var sensorsRecyclerView: RecyclerView
    private lateinit var noSensorsTextView: TextView
    private lateinit var addSensorButton: MaterialButton
    private lateinit var progressBar: ProgressBar

    private lateinit var sensorEditableAdapter: SensorEditableAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_edit_panel)

        panelId = intent.getIntExtra("PANEL_ID", -1)
        currentPanelLocation = intent.getStringExtra("PANEL_LOCATION") ?: "Panel"

        if (panelId == -1) {
            Toast.makeText(this, "Error: Panel ID not found.", Toast.LENGTH_LONG).show()
            finish()
            return
        }

        toolbar = findViewById(R.id.edit_panel_toolbar)
        setSupportActionBar(toolbar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.setDisplayShowHomeEnabled(true)
        updateToolbarTitle()


        panelNameHeaderTextView = findViewById(R.id.edit_panel_header_title)
        panelNameEditText = findViewById(R.id.edit_panel_name_edit_text)
        savePanelNameButton = findViewById(R.id.save_panel_name_button)
        sensorsRecyclerView = findViewById(R.id.edit_panel_sensors_recycler_view)
        noSensorsTextView = findViewById(R.id.edit_panel_no_sensors_text_view)
        addSensorButton = findViewById(R.id.edit_panel_add_sensor_button)
        progressBar = findViewById(R.id.edit_panel_progress_bar)

        panelNameHeaderTextView.text = getString(R.string.editing_panel_format, currentPanelLocation)
        panelNameEditText.setText(currentPanelLocation)


        setupRecyclerView()
        fetchPanelSensors()

        addSensorButton.setOnClickListener {
            showAddSensorDialog()
        }

        savePanelNameButton.setOnClickListener {
            val newName = panelNameEditText.text.toString().trim()
            if (newName.isEmpty()) {
                panelNameEditText.error = getString(R.string.panel_name_cannot_be_empty)
                return@setOnClickListener
            }
            if (newName != currentPanelLocation) {
                updatePanelName(newName)
            } else {
                Toast.makeText(this, getString(R.string.panel_name_not_changed), Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun updateToolbarTitle() {
        supportActionBar?.title = getString(R.string.edit_panel_title_format, currentPanelLocation)
    }


    private fun updatePanelName(newName: String) {
        progressBar.visibility = View.VISIBLE
        scope.launch {
            try {
                val request = UpdatePanelRequest(location = newName)
                val response = RetrofitClient.apiService.updateSolarPanel(panelId, request)
                if (response.isSuccessful && response.body() != null) {
                    currentPanelLocation = response.body()!!.location // Update current location
                    panelNameEditText.setText(currentPanelLocation)
                    panelNameHeaderTextView.text = getString(R.string.editing_panel_format, currentPanelLocation)
                    updateToolbarTitle()
                    Toast.makeText(this@EditPanelActivity, getString(R.string.panel_name_updated_successfully), Toast.LENGTH_SHORT).show()
                    // Notify HomeActivity to refresh if it's in the backstack, or rely on onResume
                    setResult(RESULT_OK) // Indicate that data changed
                } else {
                    handleApiError(response.code(), "Failed to update panel name")
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Error updating panel name for panel $panelId", e)
                Toast.makeText(this@EditPanelActivity, "Error: ${e.message}", Toast.LENGTH_LONG).show()
            } finally {
                progressBar.visibility = View.GONE
            }
        }
    }


    private fun setupRecyclerView() {
        sensorEditableAdapter = SensorEditableAdapter(this, mutableListOf()) { sensor ->
            showDeleteSensorConfirmationDialog(sensor)
        }
        sensorsRecyclerView.layoutManager = LinearLayoutManager(this)
        sensorsRecyclerView.adapter = sensorEditableAdapter
    }

    private fun fetchPanelSensors() {
        progressBar.visibility = View.VISIBLE
        scope.launch {
            try {
                val response: Response<SensorListResponse> =
                    RetrofitClient.apiService.getPanelSensors(panelId, page = 1, limit = 100)

                if (response.isSuccessful) {
                    val sensorList = response.body()?.sensors ?: emptyList()
                    if (sensorList.isEmpty()) {
                        sensorsRecyclerView.visibility = View.GONE
                        noSensorsTextView.visibility = View.VISIBLE
                    } else {
                        sensorsRecyclerView.visibility = View.VISIBLE
                        noSensorsTextView.visibility = View.GONE
                        sensorEditableAdapter.updateSensors(sensorList)
                    }
                } else {
                    handleApiError(response.code(), "Failed to fetch sensors for panel $panelId")
                    sensorsRecyclerView.visibility = View.GONE
                    noSensorsTextView.visibility = View.VISIBLE
                    noSensorsTextView.text = getString(R.string.error_loading_sensors)
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Error fetching sensors for panel $panelId", e)
                Toast.makeText(this@EditPanelActivity, "Error: ${e.message}", Toast.LENGTH_LONG).show()
                sensorsRecyclerView.visibility = View.GONE
                noSensorsTextView.visibility = View.VISIBLE
                noSensorsTextView.text = getString(R.string.error_loading_sensors)
            } finally {
                progressBar.visibility = View.GONE
            }
        }
    }

    private fun showAddSensorDialog() {
        val dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_add_sensor, null)
        val sensorTypeSpinner = dialogView.findViewById<Spinner>(R.id.dialog_sensor_type_spinner)

        val sensorTypes = arrayOf("TEMPERATURE", "POWER")
        val adapter = ArrayAdapter(this, android.R.layout.simple_spinner_dropdown_item, sensorTypes)
        sensorTypeSpinner.adapter = adapter

        AlertDialog.Builder(this)
            .setTitle(getString(R.string.add_new_sensor_title))
            .setView(dialogView)
            .setPositiveButton(getString(R.string.add_action)) { dialog, _ ->
                val selectedSensorTypeString = sensorTypeSpinner.selectedItem.toString()
                val typeId = when (selectedSensorTypeString.uppercase()) {
                    "TEMPERATURE" -> 1
                    "POWER" -> 2
                    else -> -1
                }
                if (typeId != -1) {
                    addNewSensor(typeId)
                } else {
                    Toast.makeText(this, getString(R.string.invalid_sensor_type_selected), Toast.LENGTH_SHORT).show()
                }
                dialog.dismiss()
            }
            .setNegativeButton(getString(R.string.cancel_action)) { dialog, _ ->
                dialog.dismiss()
            }
            .show()
    }

    private fun addNewSensor(typeId: Int) {
        progressBar.visibility = View.VISIBLE
        scope.launch {
            try {
                val request = CreateSensorRequest(typeId = typeId, solarPanelId = panelId)
                val response: Response<Sensor> = RetrofitClient.apiService.createSensor(request)

                if (response.isSuccessful) {
                    Toast.makeText(this@EditPanelActivity, getString(R.string.sensor_added_successfully), Toast.LENGTH_SHORT).show()
                    fetchPanelSensors() // Refresh list
                } else {
                    handleApiError(response.code(), "Failed to add new sensor")
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Error adding new sensor to panel $panelId", e)
                Toast.makeText(this@EditPanelActivity, "Error adding sensor: ${e.message}", Toast.LENGTH_LONG).show()
            } finally {
                progressBar.visibility = View.GONE
            }
        }
    }


    private fun showDeleteSensorConfirmationDialog(sensor: Sensor) {
        AlertDialog.Builder(this)
            .setTitle(getString(R.string.delete_sensor_confirmation_title))
            .setMessage(getString(R.string.delete_sensor_confirmation_message, sensor.type.name, sensor.id.toString()))
            .setIcon(R.drawable.ic_warning)
            .setPositiveButton(getString(R.string.delete_action)) { dialog, _ ->
                deleteSensor(sensor)
                dialog.dismiss()
            }
            .setNegativeButton(getString(R.string.cancel_action)) { dialog, _ ->
                dialog.dismiss()
            }
            .show()
    }

    private fun deleteSensor(sensor: Sensor) {
        progressBar.visibility = View.VISIBLE
        scope.launch {
            try {
                val response = RetrofitClient.apiService.deleteSensor(sensor.id)
                if (response.isSuccessful) {
                    Toast.makeText(this@EditPanelActivity, getString(R.string.sensor_deleted_successfully, sensor.type.name, sensor.id.toString()), Toast.LENGTH_SHORT).show()
                    fetchPanelSensors() // Refresh the list
                } else {
                    handleApiError(response.code(), "Failed to delete sensor ${sensor.id}")
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Error deleting sensor ${sensor.id}", e)
                Toast.makeText(this@EditPanelActivity, "Error deleting sensor: ${e.message}", Toast.LENGTH_LONG).show()
            } finally {
                progressBar.visibility = View.GONE
            }
        }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        if (item.itemId == android.R.id.home) {
            onBackPressedDispatcher.onBackPressed()
            return true
        }
        return super.onOptionsItemSelected(item)
    }

    private fun handleApiError(code: Int, defaultMessage: String = "An API error occurred") {
        when (code) {
            401, 403 -> {
                LoggerUtil.logWarning("API Error $code in EditPanel: Session expired or forbidden.")
                getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE).edit().clear().apply()
                val intent = Intent(this, MainActivity::class.java)
                intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                startActivity(intent)
                finishAffinity()
                Toast.makeText(this, "Session expired. Please log in again.", Toast.LENGTH_LONG).show()
            }
            else -> {
                LoggerUtil.logError("API error in EditPanel: code=$code, message: $defaultMessage")
                Toast.makeText(this, "$defaultMessage (Error: $code).", Toast.LENGTH_LONG).show()
            }
        }
    }
}

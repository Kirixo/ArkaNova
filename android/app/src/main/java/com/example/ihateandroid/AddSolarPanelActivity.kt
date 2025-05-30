package com.example.ihateandroid

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.MenuItem
import android.view.View
import android.widget.EditText
import android.widget.ProgressBar
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.button.MaterialButton
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import retrofit2.Response

class AddSolarPanelActivity : AppCompatActivity() {
    private val scope = CoroutineScope(Dispatchers.Main)

    private lateinit var panelNameEditText: EditText
    private lateinit var createPanelButton: MaterialButton
    private lateinit var progressBar: ProgressBar
    private lateinit var toolbar: MaterialToolbar

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_add_solar_panel)

        toolbar = findViewById(R.id.add_panel_toolbar)
        setSupportActionBar(toolbar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.setDisplayShowHomeEnabled(true)

        panelNameEditText = findViewById(R.id.panel_name_edit_text)
        createPanelButton = findViewById(R.id.create_panel_button)
        progressBar = findViewById(R.id.progress_bar)

        createPanelButton.setOnClickListener {
            val panelName = panelNameEditText.text.toString().trim()
            if (panelName.isEmpty()) {
                Toast.makeText(this, getString(R.string.please_enter_panel_name), Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            scope.launch {
                createSolarPanelAndProceed(panelName)
            }
        }
    }

    private suspend fun createSolarPanelAndProceed(panelName: String) {
        withContext(Dispatchers.Main) {
            progressBar.visibility = View.VISIBLE
            createPanelButton.isEnabled = false
            panelNameEditText.isEnabled = false
        }

        withContext(Dispatchers.IO) {
            try {
                val userId = getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE).getInt("userId", 0)
                if (userId == 0) {
                    withContext(Dispatchers.Main) {
                        Toast.makeText(this@AddSolarPanelActivity, "User not logged in.", Toast.LENGTH_LONG).show()
                        // Redirect to login or handle appropriately
                        startActivity(Intent(this@AddSolarPanelActivity, MainActivity::class.java).apply {
                            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                        })
                        finish()
                    }
                    return@withContext
                }

                val request = CreatePanelRequest(location = panelName, userId = userId)
                val response: Response<SolarPanel> = RetrofitClient.apiService.createPanel(request)

                withContext(Dispatchers.Main) {
                    progressBar.visibility = View.GONE
                    // Re-enable fields only if there was an error not leading to redirection
                    // createPanelButton.isEnabled = true
                    // panelNameEditText.isEnabled = true

                    if (response.isSuccessful) {
                        val panel: SolarPanel? = response.body()
                        if (panel != null) {
                            Toast.makeText(this@AddSolarPanelActivity, getString(R.string.panel_created_successfully_toast, panel.location), Toast.LENGTH_SHORT).show()
                            // Redirect to EditPanelActivity
                            val intent = Intent(this@AddSolarPanelActivity, EditPanelActivity::class.java).apply {
                                putExtra("PANEL_ID", panel.id)
                                putExtra("PANEL_LOCATION", panel.location)
                            }
                            startActivity(intent)
                            finish() // Finish AddSolarPanelActivity after redirection
                        } else {
                            createPanelButton.isEnabled = true
                            panelNameEditText.isEnabled = true
                            Toast.makeText(this@AddSolarPanelActivity, getString(R.string.failed_to_create_panel_no_data), Toast.LENGTH_LONG).show()
                        }
                    } else {
                        createPanelButton.isEnabled = true
                        panelNameEditText.isEnabled = true
                        handleApiError(response.code(), "Failed to create panel")
                    }
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    progressBar.visibility = View.GONE
                    createPanelButton.isEnabled = true
                    panelNameEditText.isEnabled = true
                    LoggerUtil.logError("Error in createSolarPanel", e)
                    Toast.makeText(this@AddSolarPanelActivity, "Error: ${e.message}", Toast.LENGTH_LONG).show()
                }
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

    private fun handleApiError(code: Int, defaultMessage: String) {
        when (code) {
            401, 403 -> {
                LoggerUtil.logWarning("API Error $code: Session expired or forbidden.")
                getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE).edit().clear().apply()
                startActivity(Intent(this, MainActivity::class.java).apply {
                    flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                })
                finish()
                Toast.makeText(this, "Session expired. Please log in again.", Toast.LENGTH_LONG).show()
            }
            else -> {
                LoggerUtil.logError("API Error $code: $defaultMessage")
                Toast.makeText(this, "$defaultMessage. Code: $code", Toast.LENGTH_LONG).show()
            }
        }
    }
}

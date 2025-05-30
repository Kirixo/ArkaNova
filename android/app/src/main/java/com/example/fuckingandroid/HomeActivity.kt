package com.example.fuckingandroid

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.google.android.material.bottomnavigation.BottomNavigationView
import com.google.android.material.button.MaterialButton
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import retrofit2.Response

class HomeActivity : AppCompatActivity() {
    private val scope = CoroutineScope(Dispatchers.Main)
    private lateinit var solarPanelAdapter: SolarPanelAdapter
    private lateinit var panelsRecyclerView: RecyclerView
    private lateinit var noPanelsTextView: TextView
    private lateinit var progressBar: ProgressBar
    private var userId: Int = 0


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_home)

        val welcomeHeader = findViewById<TextView>(R.id.welcome_header)
        panelsRecyclerView = findViewById(R.id.panels_recycler_view)
        noPanelsTextView = findViewById(R.id.no_panels_text_view)
        progressBar = findViewById(R.id.home_progress_bar)
        val addSolarPanelButton = findViewById<MaterialButton>(R.id.add_solar_panel_button)
        val bottomNav = findViewById<BottomNavigationView>(R.id.bottom_nav)

        val prefs = getSharedPreferences("ArkaNovaPrefs", MODE_PRIVATE)
        userId = prefs.getInt("userId", 0)
        val email = prefs.getString("email", "User") ?: "User"

        if (userId == 0) {
            LoggerUtil.logWarning("No userId found in SharedPreferences, redirecting to MainActivity.")
            startActivity(Intent(this, MainActivity::class.java))
            finish()
            return
        }
        welcomeHeader.text = getString(R.string.welcome_user_format, email)
        LoggerUtil.logInfo("Home screen loaded for userId=$userId, email=$email")

        setupRecyclerView()

        addSolarPanelButton.setOnClickListener {
            startActivity(Intent(this, AddSolarPanelActivity::class.java))
        }

        bottomNav.selectedItemId = R.id.nav_home
        bottomNav.setOnItemSelectedListener { item ->
            when (item.itemId) {
                R.id.nav_home -> true
                R.id.nav_dashboard -> {
                    startActivity(Intent(this, DashboardActivity::class.java))
                    true
                }
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
    }

    override fun onResume() {
        super.onResume()
        // Refresh panel list when returning to the activity
        if (userId != 0) {
            fetchUserPanels()
        }
    }

    private fun setupRecyclerView() {
        solarPanelAdapter = SolarPanelAdapter(this, emptyList()) { panel, action ->
            when (action) {
                SolarPanelAdapter.PanelAction.VIEW_DASHBOARD -> {
                    val intent = Intent(this, DashboardActivity::class.java)
                    intent.putExtra("SELECTED_PANEL_ID", panel.id)
                    startActivity(intent)
                }
                SolarPanelAdapter.PanelAction.EDIT -> {
                    val intent = Intent(this, EditPanelActivity::class.java)
                    intent.putExtra("PANEL_ID", panel.id)
                    intent.putExtra("PANEL_LOCATION", panel.location)
                    startActivity(intent)
                }
                SolarPanelAdapter.PanelAction.DELETE -> {
                    showDeletePanelConfirmationDialog(panel)
                }
            }
        }
        panelsRecyclerView.layoutManager = LinearLayoutManager(this)
        panelsRecyclerView.adapter = solarPanelAdapter
    }

    private fun fetchUserPanels() {
        progressBar.visibility = View.VISIBLE
        scope.launch {
            try {
                val response: Response<PanelListResponse> =
                    RetrofitClient.apiService.getUserPanels(userId, page = 1, limit = 100) // Fetch more panels

                if (response.isSuccessful) {
                    val panelList = response.body()?.panels ?: emptyList()
                    if (panelList.isEmpty()) {
                        panelsRecyclerView.visibility = View.GONE
                        noPanelsTextView.visibility = View.VISIBLE
                    } else {
                        panelsRecyclerView.visibility = View.VISIBLE
                        noPanelsTextView.visibility = View.GONE
                        solarPanelAdapter.updatePanels(panelList)
                    }
                } else {
                    handleApiError(response.code(), "Failed to fetch panels")
                    panelsRecyclerView.visibility = View.GONE
                    noPanelsTextView.visibility = View.VISIBLE
                    noPanelsTextView.text = getString(R.string.error_loading_panels)
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Error fetching user panels", e)
                Toast.makeText(this@HomeActivity, "Error: ${e.message}", Toast.LENGTH_LONG).show()
                panelsRecyclerView.visibility = View.GONE
                noPanelsTextView.visibility = View.VISIBLE
                noPanelsTextView.text = getString(R.string.error_loading_panels)
            } finally {
                progressBar.visibility = View.GONE
            }
        }
    }

    private fun showDeletePanelConfirmationDialog(panel: SolarPanel) {
        AlertDialog.Builder(this)
            .setTitle(getString(R.string.delete_panel_confirmation_title))
            .setMessage(getString(R.string.delete_panel_confirmation_message, panel.location))
            .setIcon(R.drawable.ic_warning) // Add a warning icon
            .setPositiveButton(getString(R.string.delete_action)) { dialog, _ ->
                deletePanel(panel)
                dialog.dismiss()
            }
            .setNegativeButton(getString(R.string.cancel_action)) { dialog, _ ->
                dialog.dismiss()
            }
            .show()
    }

    private fun deletePanel(panel: SolarPanel) {
        progressBar.visibility = View.VISIBLE
        scope.launch {
            try {
                val response = RetrofitClient.apiService.deleteSolarPanel(panel.id)
                if (response.isSuccessful) {
                    Toast.makeText(this@HomeActivity, getString(R.string.panel_deleted_successfully, panel.location), Toast.LENGTH_SHORT).show()
                    fetchUserPanels() // Refresh the list
                } else {
                    handleApiError(response.code(), "Failed to delete panel ${panel.location}")
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Error deleting panel ${panel.id}", e)
                Toast.makeText(this@HomeActivity, "Error deleting panel: ${e.message}", Toast.LENGTH_LONG).show()
            } finally {
                progressBar.visibility = View.GONE
            }
        }
    }

    private fun handleApiError(code: Int, defaultMessage: String = "An API error occurred") {
        when (code) {
            401, 403 -> {
                LoggerUtil.logWarning("API Error $code on Home: Session expired or forbidden. Logging out.")
                getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE).edit().clear().apply()
                val intent = Intent(this@HomeActivity, MainActivity::class.java)
                intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                startActivity(intent)
                finish()
                Toast.makeText(this@HomeActivity, "Session expired. Please log in again.", Toast.LENGTH_LONG).show()
            }
            else -> {
                LoggerUtil.logError("API error on Home: code=$code, message: $defaultMessage")
                Toast.makeText(this@HomeActivity, "$defaultMessage (Error: $code).", Toast.LENGTH_LONG).show()
            }
        }
    }
}

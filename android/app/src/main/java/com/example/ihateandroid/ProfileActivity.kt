package com.example.ihateandroid

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.bottomnavigation.BottomNavigationView
import com.google.android.material.button.MaterialButton
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import retrofit2.Response

class ProfileActivity : AppCompatActivity() {
    private val scope = CoroutineScope(Dispatchers.Main)
    private lateinit var toolbar: MaterialToolbar

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_profile)

        toolbar = findViewById(R.id.profile_toolbar)
        setSupportActionBar(toolbar)
        // No up arrow for profile screen usually, unless it's not a top-level destination
        // supportActionBar?.setDisplayHomeAsUpEnabled(true)
        // supportActionBar?.setDisplayShowHomeEnabled(true)

        val profileCard = findViewById<CardView>(R.id.profile_card)
        val emailText = findViewById<TextView>(R.id.email_text)
        val editButton = findViewById<MaterialButton>(R.id.edit_button)
        val logoutButton = findViewById<MaterialButton>(R.id.logout_button)
        val deleteButton = findViewById<MaterialButton>(R.id.delete_button)
        val bottomNav = findViewById<BottomNavigationView>(R.id.bottom_nav)

        try {
            val fadeIn = android.view.animation.AnimationUtils.loadAnimation(this, R.anim.fade_in)
            profileCard.startAnimation(fadeIn)
        } catch (e: Exception) {
            LoggerUtil.logWarning("Fade in animation not found or failed for profile card.")
        }

        val prefs = getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE)
        val userId = prefs.getInt("userId", 0)

        if (userId == 0) {
            LoggerUtil.logError("User ID is 0 in ProfileActivity, redirecting to login.")
            startActivity(Intent(this, MainActivity::class.java).apply{
                flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
            })
            finish()
            return
        }

        bottomNav.selectedItemId = R.id.nav_profile
        bottomNav.setOnItemSelectedListener { item ->
            when (item.itemId) {
                R.id.nav_home -> {
                    startActivity(Intent(this, HomeActivity::class.java))
                    finish()
                    true
                }
                R.id.nav_dashboard -> {
                    startActivity(Intent(this, DashboardActivity::class.java))
                    finish()
                    true
                }
                R.id.nav_profile -> true
                else -> false
            }
        }

        scope.launch {
            fetchUserDetails(userId, emailText)
        }

        editButton.setOnClickListener {
            // Assuming DialogEditProfileFragment exists and is correctly implemented
            val dialog = DialogEditProfileFragment.newInstance(userId, emailText.text.toString())
            dialog.show(supportFragmentManager, "EditProfileDialog")
        }

        logoutButton.setOnClickListener {
            prefs.edit().clear().apply()
            Toast.makeText(this, getString(R.string.logged_out_message), Toast.LENGTH_SHORT).show()
            startActivity(Intent(this, MainActivity::class.java).apply{
                flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
            })
            finishAffinity()
        }

        deleteButton.setOnClickListener {
            AlertDialog.Builder(this)
                .setTitle(getString(R.string.delete_account_dialog_title))
                .setMessage(getString(R.string.delete_account_dialog_message))
                .setIcon(R.drawable.ic_warning)
                .setPositiveButton(getString(R.string.delete_action)) { _, _ ->
                    scope.launch {
                        deleteAccount(userId)
                    }
                }
                .setNegativeButton(getString(R.string.cancel_action), null)
                .show()
        }
    }

    private suspend fun fetchUserDetails(userId: Int, emailText: TextView) {
        withContext(Dispatchers.IO) {
            try {
                val response: Response<UserResponse> = RetrofitClient.apiService.getUserDetails(userId)
                if (response.isSuccessful && response.body() != null) {
                    val user = response.body()!!
                    withContext(Dispatchers.Main) {
                        emailText.text = user.email
                    }
                } else {
                    withContext(Dispatchers.Main) {
                        handleApiError(response.code(), "Failed to fetch user details")
                        emailText.text = getString(R.string.failed_to_load_data)
                    }
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Exception in fetchUserDetails", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(this@ProfileActivity, "Error: ${e.message}", Toast.LENGTH_LONG).show()
                    emailText.text = getString(R.string.failed_to_load_data)
                }
            }
        }
    }

    private suspend fun deleteAccount(userId: Int) {
        withContext(Dispatchers.IO) {
            try {
                val response: Response<Unit> = RetrofitClient.apiService.deleteUser(userId)
                if (response.isSuccessful) {
                    withContext(Dispatchers.Main) {
                        getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE).edit().clear().apply()
                        Toast.makeText(this@ProfileActivity, getString(R.string.account_deleted_successfully), Toast.LENGTH_SHORT).show()
                        startActivity(Intent(this@ProfileActivity, MainActivity::class.java).apply{
                            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                        })
                        finishAffinity()
                    }
                } else {
                    withContext(Dispatchers.Main) {
                        handleApiError(response.code(), "Failed to delete account")
                    }
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Exception in deleteAccount", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(this@ProfileActivity, "Error: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }
        }
    }

    // If Profile is a top-level destination, up navigation might not be desired.
    // If it can be reached from other non-bottom-nav screens, enable it.
    // override fun onOptionsItemSelected(item: MenuItem): Boolean {
    //     if (item.itemId == android.R.id.home) {
    //         onBackPressedDispatcher.onBackPressed()
    //         return true
    //     }
    //     return super.onOptionsItemSelected(item)
    // }

    private fun handleApiError(code: Int, defaultMessage: String = "An API error occurred.") {
        when (code) {
            401, 403 -> {
                LoggerUtil.logWarning("API Error $code in Profile: Session expired or forbidden.")
                getSharedPreferences("ArkaNovaPrefs", MODE_PRIVATE).edit().clear().apply()
                val intent = Intent(this, MainActivity::class.java)
                intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
                startActivity(intent)
                finishAffinity()
                Toast.makeText(this, getString(R.string.session_expired_login_again), Toast.LENGTH_LONG).show()
            }
            else -> {
                LoggerUtil.logError("API error in Profile: code=$code, message: $defaultMessage")
                Toast.makeText(this, "$defaultMessage (Error: $code).", Toast.LENGTH_LONG).show()
            }
        }
    }
}

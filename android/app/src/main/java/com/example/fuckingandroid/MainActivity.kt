package com.example.fuckingandroid

import android.content.Intent
import android.os.Bundle
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import com.google.android.material.button.MaterialButton
import com.google.android.material.textfield.TextInputEditText
import com.google.android.material.textfield.TextInputLayout
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import retrofit2.Response
import android.view.animation.AnimationUtils

class MainActivity : AppCompatActivity() {
    private val scope = CoroutineScope(Dispatchers.Main)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Apply fade-in animation to card
        val cardView = findViewById<CardView>(R.id.card_view)
        val fadeIn = AnimationUtils.loadAnimation(this, R.anim.fade_in)
        cardView.startAnimation(fadeIn)

        // Initialize UI components
        val emailInput = findViewById<TextInputEditText>(R.id.email_input)
        val emailLayout = findViewById<TextInputLayout>(R.id.email_input_layout)
        val passwordInput = findViewById<TextInputEditText>(R.id.password_input)
        val passwordLayout = findViewById<TextInputLayout>(R.id.password_input_layout)
        val loginButton = findViewById<MaterialButton>(R.id.login_button)
        val registerLink = findViewById<TextView>(R.id.register_link)

        // Login button click
        loginButton.setOnClickListener {
            validateAndLogin(emailInput.text.toString(), passwordInput.text.toString())
        }

        // Register link click
        registerLink.setOnClickListener {
            startActivity(Intent(this, RegisterActivity::class.java))
        }

        // Forgot Password link click
//        forgotPasswordLink.setOnClickListener {
//            Toast.makeText(this, "Feature coming soon", Toast.LENGTH_SHORT).show()
//        }
    }

    private fun validateAndLogin(email: String, password: String) {
        val emailLayout = findViewById<TextInputLayout>(R.id.email_input_layout)
        val passwordLayout = findViewById<TextInputLayout>(R.id.password_input_layout)
        val loginButton = findViewById<MaterialButton>(R.id.login_button)

        // Reset errors
        emailLayout.error = null
        passwordLayout.error = null

        var isValid = true

        // Email validation
        if (email.isEmpty()) {
            emailLayout.error = "Please enter a valid email"
            isValid = false
        } else if (!android.util.Patterns.EMAIL_ADDRESS.matcher(email).matches()) {
            emailLayout.error = "Invalid email format"
            isValid = false
        }

        // Password validation
        if (password.isEmpty()) {
            passwordLayout.error = "Please enter a password"
            isValid = false
        } else if (password.length < 8) {
            passwordLayout.error = "Password must be at least 8 characters"
            isValid = false
        }

        if (isValid) {
            // Proceed to API call
            loginButton.isEnabled = false
            loginButton.text = "Logging in..."
            scope.launch {
                loginUser(email, password)
                loginButton.isEnabled = true
                loginButton.text = "Login"
            }
        }
    }

    private suspend fun loginUser(email: String, password: String) {
        withContext(Dispatchers.IO) {
            try {
                LoggerUtil.logInfo("Attempting login for email=$email")
                val request = LoginRequest(email, password)
                val response = RetrofitClient.apiService.login(request)
                LoggerUtil.logInfo("Login response: code=${response.code()}, body=${response.body()}")

                withContext(Dispatchers.Main) {
                    if (response.isSuccessful) {
                        // Store userId and email
                        val userId = response.body()?.id ?: 0
                        val userEmail = response.body()?.email ?: email
                        val prefs = getSharedPreferences("ArkaNovaPrefs", MODE_PRIVATE)
                        prefs.edit()
                            .putInt("userId", userId)
                            .putString("email", userEmail)
                            .apply()
                        LoggerUtil.logInfo("Login successful: userId=$userId, email=$userEmail")

                        // Navigate to Home Screen
                        Toast.makeText(this@MainActivity, "Login successful", Toast.LENGTH_SHORT).show()
                        startActivity(Intent(this@MainActivity, HomeActivity::class.java))
                        finish()
                    } else {
                        LoggerUtil.logError("Login failed: code=${response.code()}, error=${response.errorBody()?.string()}")
                        when (response.code()) {
                            401 -> Toast.makeText(
                                this@MainActivity,
                                "Login failed. Please check your credentials.",
                                Toast.LENGTH_LONG
                            ).show()
                            400 -> Toast.makeText(
                                this@MainActivity,
                                "Invalid input. Please check your email and password.",
                                Toast.LENGTH_LONG
                            ).show()
                            else -> Toast.makeText(
                                this@MainActivity,
                                "An error occurred. Please try again.",
                                Toast.LENGTH_LONG
                            ).show()
                        }
                    }
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Login exception", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(
                        this@MainActivity,
                        "Failed to login: ${e.message}",
                        Toast.LENGTH_LONG
                    ).show()
                }
            }
        }
    }
}
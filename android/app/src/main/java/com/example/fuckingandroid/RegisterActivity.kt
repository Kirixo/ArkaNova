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

class RegisterActivity : AppCompatActivity() {
    private val scope = CoroutineScope(Dispatchers.Main)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_register)

        // Apply fade-in animation to card
        val cardView = findViewById<CardView>(R.id.card_view)
        val fadeIn = AnimationUtils.loadAnimation(this, R.anim.fade_in)
        cardView.startAnimation(fadeIn)

        // Initialize UI components
        val emailInput = findViewById<TextInputEditText>(R.id.email_input)
        val emailLayout = findViewById<TextInputLayout>(R.id.email_input_layout)
        val passwordInput = findViewById<TextInputEditText>(R.id.password_input)
        val passwordLayout = findViewById<TextInputLayout>(R.id.password_input_layout)
        val confirmPasswordInput = findViewById<TextInputEditText>(R.id.confirm_password_input)
        val confirmPasswordLayout = findViewById<TextInputLayout>(R.id.confirm_password_input_layout)
        val registerButton = findViewById<MaterialButton>(R.id.register_button)
        val backToLoginLink = findViewById<TextView>(R.id.back_to_login_link)

        // Register button click
        registerButton.setOnClickListener {
            validateAndRegister(
                emailInput.text.toString(),
                passwordInput.text.toString(),
                confirmPasswordInput.text.toString()
            )
        }

        // Back to Login link click
        backToLoginLink.setOnClickListener {
            startActivity(Intent(this, MainActivity::class.java))
            finish()
        }
    }

    private fun validateAndRegister(email: String, password: String, confirmPassword: String) {
        val emailLayout = findViewById<TextInputLayout>(R.id.email_input_layout)
        val passwordLayout = findViewById<TextInputLayout>(R.id.password_input_layout)
        val confirmPasswordLayout = findViewById<TextInputLayout>(R.id.confirm_password_input_layout)
        val registerButton = findViewById<MaterialButton>(R.id.register_button)

        // Reset errors
        emailLayout.error = null
        passwordLayout.error = null
        confirmPasswordLayout.error = null

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

        // Confirm Password validation
        if (confirmPassword != password) {
            confirmPasswordLayout.error = "Passwords do not match"
            isValid = false
        }

        if (isValid) {
            // Proceed to API call
            registerButton.isEnabled = false
            registerButton.text = "Registering..."
            scope.launch {
                registerUser(email, password)
                registerButton.isEnabled = true
                registerButton.text = "Register"
            }
        }
    }

    private suspend fun registerUser(email: String, password: String) {
        withContext(Dispatchers.IO) {
            try {
                LoggerUtil.logInfo("Attempting registration for email=$email")
                val request = RegisterRequest(email, password)
                val response: Response<UserResponse> = RetrofitClient.apiService.register(request)
                LoggerUtil.logInfo("Register response: code=${response.code()}, body=${response.body()}")

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
                        LoggerUtil.logInfo("Registration successful: userId=$userId, email=$userEmail")

                        // Navigate to Home Screen
                        Toast.makeText(this@RegisterActivity, "Registration successful", Toast.LENGTH_SHORT).show()
                        startActivity(Intent(this@RegisterActivity, HomeActivity::class.java))
                        finish()
                    } else {
                        LoggerUtil.logError("Registration failed: code=${response.code()}, error=${response.errorBody()?.string()}")
                        when (response.code()) {
                            400 -> Toast.makeText(
                                this@RegisterActivity,
                                "Invalid input. Email may already be in use.",
                                Toast.LENGTH_LONG
                            ).show()
                            else -> Toast.makeText(
                                this@RegisterActivity,
                                "An error occurred. Please try again.",
                                Toast.LENGTH_LONG
                            ).show()
                        }
                    }
                }
            } catch (e: Exception) {
                LoggerUtil.logError("Registration exception", e)
                withContext(Dispatchers.Main) {
                    Toast.makeText(
                        this@RegisterActivity,
                        "Failed to register: ${e.message}",
                        Toast.LENGTH_LONG
                    ).show()
                }
            }
        }
    }
}
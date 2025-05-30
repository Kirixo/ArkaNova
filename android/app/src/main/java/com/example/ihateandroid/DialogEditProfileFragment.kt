package com.example.ihateandroid

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.util.Patterns
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast
import androidx.fragment.app.DialogFragment
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import retrofit2.Response

class DialogEditProfileFragment : DialogFragment() {
    private var userId: Int = 0
    private var currentEmail: String = ""

    companion object {
        fun newInstance(userId: Int, currentEmail: String): DialogEditProfileFragment {
            return DialogEditProfileFragment().apply {
                arguments = Bundle().apply {
                    putInt("userId", userId)
                    putString("currentEmail", currentEmail)
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        arguments?.let {
            userId = it.getInt("userId")
            currentEmail = it.getString("currentEmail", "")
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        try {
            return inflater.inflate(R.layout.dialog_edit_profile, container, false)
        } catch (e: Exception) {
            Toast.makeText(context, "Error loading dialog", Toast.LENGTH_LONG).show()
            dismiss()
            return null
        }
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val emailInput = view.findViewById<EditText>(R.id.email_input)
        val passwordInput = view.findViewById<EditText>(R.id.password_input)
        val saveButton = view.findViewById<Button>(R.id.save_button)
        val cancelButton = view.findViewById<Button>(R.id.cancel_button)
        val emailError = view.findViewById<TextView>(R.id.email_error)
        val progressBar = view.findViewById<ProgressBar>(R.id.progress_bar)

        // Initialize fields
        emailInput.setText(currentEmail)
        saveButton.isEnabled = false
        progressBar.visibility = View.GONE

        // Email validation
        emailInput.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable?) {
                val email = s.toString().trim()
                if (email.isEmpty() || !Patterns.EMAIL_ADDRESS.matcher(email).matches()) {
                    emailError.visibility = View.VISIBLE
                    emailError.text = "Invalid email address"
                    saveButton.isEnabled = false
                } else {
                    emailError.visibility = View.GONE
                    saveButton.isEnabled = email != currentEmail || passwordInput.text.isNotBlank()
                }
            }
        })

        // Password validation
        passwordInput.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable?) {
                val email = emailInput.text.toString().trim()
                saveButton.isEnabled =
                    (email.isNotEmpty() && Patterns.EMAIL_ADDRESS.matcher(email).matches()) &&
                            (email != currentEmail || s.toString().isNotBlank())
            }
        })

        // Save changes
        saveButton.setOnClickListener {
            progressBar.visibility = View.VISIBLE
            saveButton.isEnabled = false
            val email = emailInput.text.toString().trim()
            val password = passwordInput.text.toString().takeIf { it.isNotBlank() }
            CoroutineScope(Dispatchers.Main).launch {
                updateProfile(userId, email, password, progressBar, saveButton)
            }
        }

        // Cancel
        cancelButton.setOnClickListener {
            dismiss()
        }
    }

    override fun onStart() {
        super.onStart()
        dialog?.window?.setLayout(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.WRAP_CONTENT
        )
    }

    private suspend fun updateProfile(
        userId: Int,
        email: String,
        password: String?,
        progressBar: ProgressBar,
        saveButton: Button
    ) {
        withContext(Dispatchers.IO) {
            try {
                val request = UpdateUserRequest(email, password)
                val response: Response<Unit> =
                    RetrofitClient.apiService.updateUser(userId, request)

                withContext(Dispatchers.Main) {
                    progressBar.visibility = View.GONE
                    saveButton.isEnabled = true

                    if (!response.isSuccessful) {
                        val errorMessage = when (response.code()) {
                            400 -> "Invalid email or password"
                            401, 403 -> "Session expired. Please log in again."
                            else -> "Failed to update profile"
                        }
                        Toast.makeText(context, errorMessage, Toast.LENGTH_LONG).show()

                        if (response.code() in setOf(401, 403)) {
                            requireActivity().getSharedPreferences("ArkaNovaPrefs", Context.MODE_PRIVATE)
                                .edit().clear().apply()
                            startActivity(Intent(requireContext(), MainActivity::class.java))
                            requireActivity().finish()
                        }
                        return@withContext
                    }

                    Toast.makeText(context, "Profile updated", Toast.LENGTH_SHORT).show()
                    // Update ProfileActivity
                    (activity as? ProfileActivity)?.let {
                        it.findViewById<TextView>(R.id.email_input)?.text = email
                    }
                    dismiss()
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    progressBar.visibility = View.GONE
                    saveButton.isEnabled = true
                    Toast.makeText(context, "Error: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }
        }
    }
}
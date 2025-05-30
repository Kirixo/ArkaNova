package com.example.fuckingandroid

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class SensorEditableAdapter(
    private val context: Context,
    private var sensors: MutableList<Sensor>,
    private val onDeleteClick: (sensor: Sensor) -> Unit
) : RecyclerView.Adapter<SensorEditableAdapter.SensorViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): SensorViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.list_item_sensor_editable, parent, false)
        return SensorViewHolder(view)
    }

    override fun onBindViewHolder(holder: SensorViewHolder, position: Int) {
        val sensor = sensors[position]
        holder.bind(sensor)
    }

    override fun getItemCount(): Int = sensors.size

    fun updateSensors(newSensors: List<Sensor>) {
        sensors.clear()
        sensors.addAll(newSensors)
        notifyDataSetChanged() // Consider DiffUtil
    }

    inner class SensorViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val sensorIcon: ImageView = itemView.findViewById(R.id.sensor_item_icon)
        private val sensorTypeTextView: TextView = itemView.findViewById(R.id.sensor_item_type_text_view)
        private val sensorIdTextView: TextView = itemView.findViewById(R.id.sensor_item_id_text_view)
        private val deleteButton: ImageButton = itemView.findViewById(R.id.sensor_item_delete_button)

        fun bind(sensor: Sensor) {
            sensorTypeTextView.text = context.getString(R.string.sensor_type_format, sensor.type.name.replaceFirstChar { if (it.isLowerCase()) it.titlecase() else it.toString() })
            sensorIdTextView.text = context.getString(R.string.sensor_id_template, sensor.id.toString())

            // Set icon based on sensor type
            when (sensor.type.name.lowercase()) {
                "temperature" -> sensorIcon.setImageResource(R.drawable.ic_thermometer)
                "power" -> sensorIcon.setImageResource(R.drawable.ic_lightning)
                else -> sensorIcon.setImageResource(R.drawable.ic_generic_sensor) // Add a generic sensor icon
            }

            deleteButton.setOnClickListener {
                onDeleteClick(sensor)
            }
        }
    }
}

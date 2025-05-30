package com.example.fuckingandroid

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.widget.PopupMenu
import androidx.recyclerview.widget.RecyclerView

class SolarPanelAdapter(
    private val context: Context,
    private var panels: List<SolarPanel>,
    private val onActionSelected: (panel: SolarPanel, action: PanelAction) -> Unit
) : RecyclerView.Adapter<SolarPanelAdapter.PanelViewHolder>() {

    enum class PanelAction { VIEW_DASHBOARD, EDIT, DELETE }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): PanelViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.list_item_solar_panel, parent, false)
        return PanelViewHolder(view)
    }

    override fun onBindViewHolder(holder: PanelViewHolder, position: Int) {
        val panel = panels[position]
        holder.bind(panel)
    }

    override fun getItemCount(): Int = panels.size

    fun updatePanels(newPanels: List<SolarPanel>) {
        panels = newPanels
        notifyDataSetChanged() // Consider using DiffUtil for better performance
    }

    inner class PanelViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val panelNameTextView: TextView = itemView.findViewById(R.id.panel_name_text_view)
        private val panelIdTextView: TextView = itemView.findViewById(R.id.panel_id_text_view)
        private val panelMenuButton: ImageView = itemView.findViewById(R.id.panel_menu_button)

        fun bind(panel: SolarPanel) {
            panelNameTextView.text = panel.location // API 'location' is used as name
            panelIdTextView.text = context.getString(R.string.panel_id_template, panel.id.toString())

            // Handle item click to view dashboard (optional, if whole item is clickable)
            itemView.setOnClickListener {
                onActionSelected(panel, PanelAction.VIEW_DASHBOARD)
            }

            panelMenuButton.setOnClickListener { view ->
                showPopupMenu(view, panel)
            }
        }

        private fun showPopupMenu(view: View, panel: SolarPanel) {
            val popup = PopupMenu(context, view)
            popup.menuInflater.inflate(R.menu.panel_item_menu, popup.menu) // Create this menu resource
            popup.setOnMenuItemClickListener { menuItem ->
                when (menuItem.itemId) {
                    R.id.action_view_dashboard -> {
                        onActionSelected(panel, PanelAction.VIEW_DASHBOARD)
                        true
                    }
                    R.id.action_edit_panel -> {
                        onActionSelected(panel, PanelAction.EDIT)
                        true
                    }
                    R.id.action_delete_panel -> {
                        onActionSelected(panel, PanelAction.DELETE)
                        true
                    }
                    else -> false
                }
            }
            popup.show()
        }
    }
}

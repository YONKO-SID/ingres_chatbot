import React from 'react'
import { motion } from 'framer-motion'

interface SidebarProps {
  onClose: () => void
  activeView: 'chat' | 'analytics' | 'map'
  onViewChange: (view: 'chat' | 'analytics' | 'map') => void
}

const Sidebar: React.FC<SidebarProps> = ({ onClose, activeView, onViewChange }) => {
  const menuItems = [
    { id: 'chat', icon: '💬', label: 'Chat Interface', description: 'AI-powered conversations' },
    { id: 'analytics', icon: '📊', label: 'Analytics', description: 'Data insights & trends' },
    { id: 'map', icon: '🗺️', label: 'Water Map', description: 'Geographic visualization' },
  ]

  const quickStats = [
    { label: 'Active Queries', value: '1,247', trend: '+12%' },
    { label: 'Data Points', value: '50K+', trend: '+5%' },
    { label: 'Accuracy', value: '95.2%', trend: '+2%' },
  ]

  return (
    <>
      {/* Backdrop */}
      <motion.div
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        exit={{ opacity: 0 }}
        onClick={onClose}
        className="absolute inset-0 z-30 bg-black/50 backdrop-blur-sm"
      />

      {/* Sidebar */}
      <motion.div
        initial={{ x: -400, opacity: 0 }}
        animate={{ x: 0, opacity: 1 }}
        exit={{ x: -400, opacity: 0 }}
        transition={{ type: "spring", damping: 25, stiffness: 200 }}
        className="absolute left-0 top-0 bottom-0 z-40 w-80 bg-slate-900/95 backdrop-blur-md border-r border-cyan-500/30"
      >
        <div className="p-6">
          {/* Header */}
          <div className="flex items-center justify-between mb-8">
            <h2 className="text-xl font-bold text-white">Navigation</h2>
            <motion.button
              whileHover={{ scale: 1.1 }}
              whileTap={{ scale: 0.9 }}
              onClick={onClose}
              className="p-2 rounded-lg bg-slate-700/50 hover:bg-slate-600/50 transition-colors"
            >
              <svg className="w-5 h-5 text-gray-300" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
              </svg>
            </motion.button>
          </div>

          {/* Menu Items */}
          <div className="space-y-3 mb-8">
            {menuItems.map((item, index) => (
              <motion.button
                key={item.id}
                initial={{ x: -50, opacity: 0 }}
                animate={{ x: 0, opacity: 1 }}
                transition={{ delay: index * 0.1 }}
                whileHover={{ scale: 1.02, x: 5 }}
                whileTap={{ scale: 0.98 }}
                onClick={() => onViewChange(item.id as any)}
                className={`w-full p-4 rounded-xl text-left transition-all duration-200 ${
                  activeView === item.id
                    ? 'bg-cyan-500/20 border border-cyan-500/50 text-cyan-300'
                    : 'bg-slate-800/50 border border-slate-700/50 text-gray-300 hover:bg-slate-700/50'
                }`}
              >
                <div className="flex items-center space-x-3">
                  <span className="text-2xl">{item.icon}</span>
                  <div>
                    <div className="font-semibold">{item.label}</div>
                    <div className="text-xs opacity-70">{item.description}</div>
                  </div>
                </div>
              </motion.button>
            ))}
          </div>

          {/* Quick Stats */}
          <div className="bg-slate-800/50 rounded-xl p-4 border border-slate-700/50">
            <h3 className="text-cyan-300 font-semibold mb-4">System Stats</h3>
            <div className="space-y-3">
              {quickStats.map((stat, index) => (
                <motion.div
                  key={stat.label}
                  initial={{ y: 20, opacity: 0 }}
                  animate={{ y: 0, opacity: 1 }}
                  transition={{ delay: 0.3 + index * 0.1 }}
                  className="flex items-center justify-between"
                >
                  <div>
                    <div className="text-sm text-gray-300">{stat.label}</div>
                    <div className="text-lg font-bold text-white">{stat.value}</div>
                  </div>
                  <div className="text-xs text-green-400">{stat.trend}</div>
                </motion.div>
              ))}
            </div>
          </div>

          {/* Footer */}
          <div className="mt-8 pt-6 border-t border-slate-700/50">
            <div className="text-xs text-gray-400 text-center">
              <p>INGRES v2.0</p>
              <p>Smart India Hackathon 2025</p>
            </div>
          </div>
        </div>
      </motion.div>
    </>
  )
}

export default Sidebar
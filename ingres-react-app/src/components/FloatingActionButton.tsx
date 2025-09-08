import React, { useState } from 'react'
import { motion, AnimatePresence } from 'framer-motion'

interface FloatingAction {
  icon: string
  label: string
  action: () => void
  color: string
}

const FloatingActionButton: React.FC = () => {
  const [isOpen, setIsOpen] = useState(false)

  const actions: FloatingAction[] = [
    {
      icon: '📊',
      label: 'Generate Report',
      action: () => console.log('Generate Report'),
      color: 'bg-blue-500'
    },
    {
      icon: '🗺️',
      label: 'View Map',
      action: () => console.log('View Map'),
      color: 'bg-green-500'
    },
    {
      icon: '⚠️',
      label: 'Alerts',
      action: () => console.log('Alerts'),
      color: 'bg-red-500'
    },
    {
      icon: '💧',
      label: 'Water Quality',
      action: () => console.log('Water Quality'),
      color: 'bg-cyan-500'
    }
  ]

  return (
    <div className="fixed bottom-6 right-6 z-50">
      {/* Action Items */}
      <AnimatePresence>
        {isOpen && (
          <div className="absolute bottom-16 right-0 space-y-3">
            {actions.map((action, index) => (
              <motion.div
                key={action.label}
                initial={{ opacity: 0, scale: 0, x: 20 }}
                animate={{ opacity: 1, scale: 1, x: 0 }}
                exit={{ opacity: 0, scale: 0, x: 20 }}
                transition={{ delay: index * 0.1 }}
                className="flex items-center space-x-3"
              >
                <motion.div
                  whileHover={{ scale: 1.1 }}
                  className="bg-slate-800/90 backdrop-blur-sm px-3 py-2 rounded-lg border border-slate-600/50 text-sm text-white whitespace-nowrap"
                >
                  {action.label}
                </motion.div>
                <motion.button
                  whileHover={{ scale: 1.1 }}
                  whileTap={{ scale: 0.9 }}
                  onClick={action.action}
                  className={`w-12 h-12 ${action.color} rounded-full flex items-center justify-center text-white shadow-lg hover:shadow-xl transition-shadow`}
                >
                  <span className="text-lg">{action.icon}</span>
                </motion.button>
              </motion.div>
            ))}
          </div>
        )}
      </AnimatePresence>

      {/* Main FAB */}
      <motion.button
        whileHover={{ scale: 1.1 }}
        whileTap={{ scale: 0.9 }}
        onClick={() => setIsOpen(!isOpen)}
        className="w-14 h-14 bg-gradient-to-r from-cyan-500 to-blue-500 rounded-full flex items-center justify-center text-white shadow-lg hover:shadow-xl transition-all duration-200"
      >
        <motion.div
          animate={{ rotate: isOpen ? 45 : 0 }}
          transition={{ duration: 0.2 }}
        >
          <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
          </svg>
        </motion.div>
      </motion.button>

      {/* Backdrop */}
      <AnimatePresence>
        {isOpen && (
          <motion.div
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            onClick={() => setIsOpen(false)}
            className="fixed inset-0 bg-black/20 backdrop-blur-sm -z-10"
          />
        )}
      </AnimatePresence>
    </div>
  )
}

export default FloatingActionButton
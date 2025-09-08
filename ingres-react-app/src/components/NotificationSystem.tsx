import React, { useState, useEffect } from 'react'
import { motion, AnimatePresence } from 'framer-motion'

interface Notification {
  id: string
  type: 'info' | 'success' | 'warning' | 'error'
  title: string
  message: string
  duration?: number
}

const NotificationSystem: React.FC = () => {
  const [notifications, setNotifications] = useState<Notification[]>([])

  // Simulate real-time notifications
  useEffect(() => {
    const interval = setInterval(() => {
      const sampleNotifications = [
        {
          type: 'info' as const,
          title: 'Data Update',
          message: 'New groundwater data available for Maharashtra region'
        },
        {
          type: 'warning' as const,
          title: 'Water Level Alert',
          message: 'Critical water levels detected in 3 districts'
        },
        {
          type: 'success' as const,
          title: 'Report Generated',
          message: 'Monthly analysis report has been successfully created'
        },
        {
          type: 'error' as const,
          title: 'Connection Issue',
          message: 'Temporary connectivity issue with sensor network'
        }
      ]

      if (Math.random() > 0.7) { // 30% chance every 10 seconds
        const notification = sampleNotifications[Math.floor(Math.random() * sampleNotifications.length)]
        addNotification({
          ...notification,
          id: `notif_${Date.now()}`,
          duration: 5000
        })
      }
    }, 10000)

    return () => clearInterval(interval)
  }, [])

  const addNotification = (notification: Notification) => {
    setNotifications(prev => [...prev, notification])

    if (notification.duration) {
      setTimeout(() => {
        removeNotification(notification.id)
      }, notification.duration)
    }
  }

  const removeNotification = (id: string) => {
    setNotifications(prev => prev.filter(n => n.id !== id))
  }

  const getNotificationStyles = (type: Notification['type']) => {
    switch (type) {
      case 'info':
        return 'bg-blue-500/20 border-blue-500/50 text-blue-300'
      case 'success':
        return 'bg-green-500/20 border-green-500/50 text-green-300'
      case 'warning':
        return 'bg-yellow-500/20 border-yellow-500/50 text-yellow-300'
      case 'error':
        return 'bg-red-500/20 border-red-500/50 text-red-300'
    }
  }

  const getNotificationIcon = (type: Notification['type']) => {
    switch (type) {
      case 'info': return 'ℹ️'
      case 'success': return '✅'
      case 'warning': return '⚠️'
      case 'error': return '❌'
    }
  }

  return (
    <div className="fixed top-20 right-4 z-50 space-y-2 max-w-sm">
      <AnimatePresence>
        {notifications.map((notification) => (
          <motion.div
            key={notification.id}
            initial={{ opacity: 0, x: 300, scale: 0.8 }}
            animate={{ opacity: 1, x: 0, scale: 1 }}
            exit={{ opacity: 0, x: 300, scale: 0.8 }}
            transition={{ type: "spring", damping: 25, stiffness: 200 }}
            className={`p-4 rounded-xl border backdrop-blur-md shadow-lg ${getNotificationStyles(notification.type)}`}
          >
            <div className="flex items-start space-x-3">
              <div className="text-lg flex-shrink-0">
                {getNotificationIcon(notification.type)}
              </div>
              <div className="flex-1 min-w-0">
                <div className="font-semibold text-sm mb-1">
                  {notification.title}
                </div>
                <div className="text-xs opacity-90 leading-relaxed">
                  {notification.message}
                </div>
              </div>
              <motion.button
                whileHover={{ scale: 1.1 }}
                whileTap={{ scale: 0.9 }}
                onClick={() => removeNotification(notification.id)}
                className="text-gray-400 hover:text-white transition-colors flex-shrink-0"
              >
                <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                </svg>
              </motion.button>
            </div>
          </motion.div>
        ))}
      </AnimatePresence>
    </div>
  )
}

export default NotificationSystem
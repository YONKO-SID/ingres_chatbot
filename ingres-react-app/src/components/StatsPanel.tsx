import React, { useState, useEffect } from 'react'
import { motion } from 'framer-motion'

interface StatCard {
  title: string
  value: string
  change: string
  trend: 'up' | 'down' | 'stable'
  icon: string
  color: string
}

const StatsPanel: React.FC = () => {
  const [stats, setStats] = useState<StatCard[]>([
    {
      title: 'Water Level',
      value: '12.5m',
      change: '+2.3%',
      trend: 'up',
      icon: '🌊',
      color: 'blue'
    },
    {
      title: 'Quality Index',
      value: '8.2/10',
      change: '+0.5',
      trend: 'up',
      icon: '💧',
      color: 'green'
    },
    {
      title: 'Extraction Rate',
      value: '45.2%',
      change: '-1.2%',
      trend: 'down',
      icon: '⚡',
      color: 'orange'
    },
    {
      title: 'Recharge Rate',
      value: '78.9%',
      change: '+5.1%',
      trend: 'up',
      icon: '🔄',
      color: 'purple'
    }
  ])

  const [currentTime, setCurrentTime] = useState(new Date())

  useEffect(() => {
    const timer = setInterval(() => {
      setCurrentTime(new Date())
    }, 1000)

    return () => clearInterval(timer)
  }, [])

  const getTrendIcon = (trend: 'up' | 'down' | 'stable') => {
    switch (trend) {
      case 'up': return '↗️'
      case 'down': return '↘️'
      case 'stable': return '➡️'
    }
  }

  const getTrendColor = (trend: 'up' | 'down' | 'stable') => {
    switch (trend) {
      case 'up': return 'text-green-400'
      case 'down': return 'text-red-400'
      case 'stable': return 'text-yellow-400'
    }
  }

  return (
    <div className="space-y-4">
      {/* Header */}
      <motion.div
        initial={{ y: -20, opacity: 0 }}
        animate={{ y: 0, opacity: 1 }}
        className="bg-slate-800/60 backdrop-blur-md rounded-xl p-4 border border-cyan-500/30"
      >
        <div className="flex items-center justify-between">
          <h2 className="text-cyan-300 font-semibold">Live Monitoring</h2>
          <div className="text-xs text-gray-400">
            {currentTime.toLocaleTimeString()}
          </div>
        </div>
        <div className="text-xs text-gray-500 mt-1">
          Last updated: {currentTime.toLocaleString()}
        </div>
      </motion.div>

      {/* Stats Cards */}
      <div className="space-y-3">
        {stats.map((stat, index) => (
          <motion.div
            key={stat.title}
            initial={{ x: -50, opacity: 0 }}
            animate={{ x: 0, opacity: 1 }}
            transition={{ delay: index * 0.1 }}
            whileHover={{ scale: 1.02, x: 5 }}
            className="bg-slate-800/60 backdrop-blur-md rounded-xl p-4 border border-slate-700/50 hover:border-cyan-500/30 transition-all duration-200"
          >
            <div className="flex items-center justify-between">
              <div className="flex items-center space-x-3">
                <div className={`w-10 h-10 rounded-lg bg-${stat.color}-500/20 flex items-center justify-center`}>
                  <span className="text-lg">{stat.icon}</span>
                </div>
                <div>
                  <div className="text-sm text-gray-300">{stat.title}</div>
                  <div className="text-xl font-bold text-white">{stat.value}</div>
                </div>
              </div>
              <div className="text-right">
                <div className={`text-sm font-semibold ${getTrendColor(stat.trend)}`}>
                  {getTrendIcon(stat.trend)} {stat.change}
                </div>
              </div>
            </div>
          </motion.div>
        ))}
      </div>

      {/* System Status */}
      <motion.div
        initial={{ y: 20, opacity: 0 }}
        animate={{ y: 0, opacity: 1 }}
        transition={{ delay: 0.5 }}
        className="bg-slate-800/60 backdrop-blur-md rounded-xl p-4 border border-slate-700/50"
      >
        <h3 className="text-cyan-300 font-semibold mb-3">System Health</h3>
        <div className="space-y-2">
          {[
            { name: 'API Response', status: 'Optimal', color: 'green' },
            { name: 'Database', status: 'Connected', color: 'green' },
            { name: 'AI Engine', status: 'Active', color: 'blue' },
            { name: 'Data Sync', status: 'Syncing', color: 'yellow' },
          ].map((item, index) => (
            <div key={item.name} className="flex items-center justify-between">
              <span className="text-sm text-gray-300">{item.name}</span>
              <div className="flex items-center space-x-2">
                <div className={`w-2 h-2 bg-${item.color}-400 rounded-full animate-pulse`}></div>
                <span className={`text-xs text-${item.color}-400`}>{item.status}</span>
              </div>
            </div>
          ))}
        </div>
      </motion.div>

      {/* Quick Insights */}
      <motion.div
        initial={{ y: 20, opacity: 0 }}
        animate={{ y: 0, opacity: 1 }}
        transition={{ delay: 0.7 }}
        className="bg-gradient-to-r from-cyan-500/10 to-blue-500/10 backdrop-blur-md rounded-xl p-4 border border-cyan-500/30"
      >
        <h3 className="text-cyan-300 font-semibold mb-2">💡 AI Insight</h3>
        <p className="text-sm text-gray-300">
          Water levels are showing positive trends in 73% of monitored regions. 
          Recommend increasing monitoring frequency in critical zones.
        </p>
      </motion.div>
    </div>
  )
}

export default StatsPanel
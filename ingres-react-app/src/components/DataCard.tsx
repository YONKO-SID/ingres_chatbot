import React from 'react'
import { motion } from 'framer-motion'

interface DataCardProps {
  title: string
  value: string
  unit?: string
  trend?: 'up' | 'down' | 'stable'
  trendValue?: string
  icon: string
  color: 'blue' | 'green' | 'orange' | 'red' | 'purple'
  description?: string
}

const DataCard: React.FC<DataCardProps> = ({
  title,
  value,
  unit,
  trend,
  trendValue,
  icon,
  color,
  description
}) => {
  const colorClasses = {
    blue: 'from-blue-500/20 to-cyan-500/20 border-blue-500/30 text-blue-300',
    green: 'from-green-500/20 to-emerald-500/20 border-green-500/30 text-green-300',
    orange: 'from-orange-500/20 to-yellow-500/20 border-orange-500/30 text-orange-300',
    red: 'from-red-500/20 to-pink-500/20 border-red-500/30 text-red-300',
    purple: 'from-purple-500/20 to-indigo-500/20 border-purple-500/30 text-purple-300'
  }

  const getTrendIcon = () => {
    switch (trend) {
      case 'up': return '↗️'
      case 'down': return '↘️'
      case 'stable': return '➡️'
      default: return ''
    }
  }

  const getTrendColor = () => {
    switch (trend) {
      case 'up': return 'text-green-400'
      case 'down': return 'text-red-400'
      case 'stable': return 'text-yellow-400'
      default: return 'text-gray-400'
    }
  }

  return (
    <motion.div
      whileHover={{ scale: 1.02, y: -2 }}
      whileTap={{ scale: 0.98 }}
      className={`relative p-4 rounded-xl bg-gradient-to-br ${colorClasses[color]} border backdrop-blur-sm overflow-hidden group cursor-pointer`}
    >
      {/* Background animation */}
      <div className="absolute inset-0 bg-gradient-to-r from-transparent via-white/5 to-transparent -translate-x-full group-hover:translate-x-full transition-transform duration-1000" />
      
      <div className="relative z-10">
        {/* Header */}
        <div className="flex items-center justify-between mb-3">
          <div className="text-2xl">{icon}</div>
          {trend && (
            <div className={`flex items-center space-x-1 text-sm ${getTrendColor()}`}>
              <span>{getTrendIcon()}</span>
              <span>{trendValue}</span>
            </div>
          )}
        </div>

        {/* Value */}
        <div className="mb-2">
          <div className="text-2xl font-bold text-white">
            {value}
            {unit && <span className="text-lg text-gray-300 ml-1">{unit}</span>}
          </div>
          <div className="text-sm opacity-80">{title}</div>
        </div>

        {/* Description */}
        {description && (
          <div className="text-xs opacity-70 mt-2">
            {description}
          </div>
        )}

        {/* Pulse indicator */}
        <div className="absolute top-2 right-2 w-2 h-2 bg-current rounded-full animate-pulse opacity-60" />
      </div>
    </motion.div>
  )
}

export default DataCard
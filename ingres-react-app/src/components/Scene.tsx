import { useFrame } from '@react-three/fiber'
import { useRef, useMemo } from 'react'
import { Sphere, Plane, Float } from '@react-three/drei'
import * as THREE from 'three'

// Enhanced water shader for groundwater visualization
const waterVertexShader = `
  varying vec2 vUv;
  varying vec3 vPosition;
  uniform float u_time;
  uniform float u_waveHeight;
  uniform float u_waveFrequency;

  void main() {
    vUv = uv;
    vPosition = position;
    
    vec3 pos = position;
    
    // Create multiple wave layers for realistic water movement
    float wave1 = sin(pos.x * u_waveFrequency + u_time) * u_waveHeight;
    float wave2 = sin(pos.y * u_waveFrequency * 0.8 + u_time * 1.2) * u_waveHeight * 0.5;
    float wave3 = sin((pos.x + pos.y) * u_waveFrequency * 0.6 + u_time * 0.8) * u_waveHeight * 0.3;
    
    pos.z += wave1 + wave2 + wave3;
    
    gl_Position = projectionMatrix * modelViewMatrix * vec4(pos, 1.0);
  }
`

const waterFragmentShader = `
  varying vec2 vUv;
  varying vec3 vPosition;
  uniform float u_time;
  uniform vec3 u_color1;
  uniform vec3 u_color2;
  uniform vec3 u_color3;

  void main() {
    // Create depth-based color gradient
    float depth = (vPosition.z + 1.0) * 0.5;
    
    // Animated color mixing
    float mixer1 = sin(vUv.x * 10.0 + u_time) * 0.5 + 0.5;
    float mixer2 = sin(vUv.y * 8.0 + u_time * 1.3) * 0.5 + 0.5;
    
    vec3 color = mix(u_color1, u_color2, mixer1);
    color = mix(color, u_color3, mixer2 * depth);
    
    // Add some transparency and glow effect
    float alpha = 0.7 + sin(u_time + vUv.x * 5.0) * 0.1;
    
    gl_FragColor = vec4(color, alpha);
  }
`

// Particle system for water droplets
function WaterParticles() {
  const particlesRef = useRef<THREE.Points>(null!)
  
  const particles = useMemo(() => {
    const positions = new Float32Array(200 * 3)
    const velocities = new Float32Array(200 * 3)
    
    for (let i = 0; i < 200; i++) {
      positions[i * 3] = (Math.random() - 0.5) * 20
      positions[i * 3 + 1] = Math.random() * 10
      positions[i * 3 + 2] = (Math.random() - 0.5) * 20
      
      velocities[i * 3] = (Math.random() - 0.5) * 0.02
      velocities[i * 3 + 1] = -Math.random() * 0.05 - 0.01
      velocities[i * 3 + 2] = (Math.random() - 0.5) * 0.02
    }
    
    return { positions, velocities }
  }, [])

  useFrame(() => {
    if (particlesRef.current) {
      const positions = particlesRef.current.geometry.attributes.position.array as Float32Array
      
      for (let i = 0; i < 200; i++) {
        positions[i * 3] += particles.velocities[i * 3]
        positions[i * 3 + 1] += particles.velocities[i * 3 + 1]
        positions[i * 3 + 2] += particles.velocities[i * 3 + 2]
        
        // Reset particles that fall too low
        if (positions[i * 3 + 1] < -5) {
          positions[i * 3 + 1] = 10
          positions[i * 3] = (Math.random() - 0.5) * 20
          positions[i * 3 + 2] = (Math.random() - 0.5) * 20
        }
      }
      
      particlesRef.current.geometry.attributes.position.needsUpdate = true
    }
  })

  return (
    <points ref={particlesRef}>
      <bufferGeometry>
        <bufferAttribute
          attach="attributes-position"
          count={200}
          array={particles.positions}
          itemSize={3}
        />
      </bufferGeometry>
      <pointsMaterial
        size={0.05}
        color="#4FC3F7"
        transparent
        opacity={0.6}
        sizeAttenuation
      />
    </points>
  )
}

// Floating data visualization spheres
function DataSpheres() {
  const spheres = useMemo(() => {
    return Array.from({ length: 8 }, (_, i) => ({
      position: [
        (Math.random() - 0.5) * 15,
        Math.random() * 5 + 2,
        (Math.random() - 0.5) * 15
      ] as [number, number, number],
      color: ['#4FC3F7', '#81C784', '#FFB74D', '#F06292'][i % 4],
      scale: Math.random() * 0.3 + 0.2
    }))
  }, [])

  return (
    <>
      {spheres.map((sphere, i) => (
        <Float
          key={i}
          speed={1 + Math.random()}
          rotationIntensity={0.5}
          floatIntensity={0.5}
        >
          <Sphere
            position={sphere.position}
            args={[sphere.scale, 16, 16]}
          >
            <meshStandardMaterial
              color={sphere.color}
              transparent
              opacity={0.7}
              emissive={sphere.color}
              emissiveIntensity={0.2}
            />
          </Sphere>
        </Float>
      ))}
    </>
  )
}

function Scene() {
  const waterRef = useRef<THREE.Mesh>(null!)
  const materialRef = useRef<THREE.ShaderMaterial>(null!)

  useFrame(({ clock }) => {
    if (materialRef.current) {
      materialRef.current.uniforms.u_time.value = clock.getElapsedTime()
    }
  })

  return (
    <group>
      {/* Main water surface */}
      <mesh ref={waterRef} rotation={[-Math.PI / 2, 0, 0]} position={[0, -2, 0]}>
        <planeGeometry args={[20, 20, 128, 128]} />
        <shaderMaterial
          ref={materialRef}
          vertexShader={waterVertexShader}
          fragmentShader={waterFragmentShader}
          uniforms={{
            u_time: { value: 0.0 },
            u_waveHeight: { value: 0.3 },
            u_waveFrequency: { value: 2.0 },
            u_color1: { value: new THREE.Color('#1E3A8A') },
            u_color2: { value: new THREE.Color('#3B82F6') },
            u_color3: { value: new THREE.Color('#06B6D4') },
          }}
          transparent
          side={THREE.DoubleSide}
        />
      </mesh>

      {/* Underground layers visualization */}
      <Plane
        rotation={[-Math.PI / 2, 0, 0]}
        position={[0, -3, 0]}
        args={[20, 20]}
      >
        <meshStandardMaterial
          color="#8B4513"
          transparent
          opacity={0.4}
        />
      </Plane>

      <Plane
        rotation={[-Math.PI / 2, 0, 0]}
        position={[0, -4, 0]}
        args={[20, 20]}
      >
        <meshStandardMaterial
          color="#654321"
          transparent
          opacity={0.6}
        />
      </Plane>

      {/* Floating data visualization */}
      <DataSpheres />

      {/* Particle system */}
      <WaterParticles />

      {/* Ambient elements */}
      <Float speed={0.5} rotationIntensity={0.2} floatIntensity={0.3}>
        <group position={[5, 2, -5]}>
          <Sphere args={[0.1, 8, 8]}>
            <meshStandardMaterial
              color="#4FC3F7"
              emissive="#4FC3F7"
              emissiveIntensity={0.5}
            />
          </Sphere>
        </group>
      </Float>

      <Float speed={0.7} rotationIntensity={0.3} floatIntensity={0.4}>
        <group position={[-6, 3, 4]}>
          <Sphere args={[0.15, 8, 8]}>
            <meshStandardMaterial
              color="#81C784"
              emissive="#81C784"
              emissiveIntensity={0.3}
            />
          </Sphere>
        </group>
      </Float>
    </group>
  )
}

export default Scene
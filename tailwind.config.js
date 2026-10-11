/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    './app/**/*.{js,jsx}',
    './components/**/*.{js,jsx}',
    './context/**/*.{js,jsx}',
  ],
  theme: {
    extend: {
      colors: {
        bg: '#F5F7F8',
        surface: '#FFFFFF',
        surfaceAlt: '#EEF2F3',
        border: '#DCE3E5',
        borderStrong: '#C4CDD0',
        ink: '#16232C',
        inkMuted: '#5B6B72',
        inkFaint: '#8B979C',
        brand: '#0F5C6B',
        brandDark: '#0A3F49',
        brandTint: '#E4EFEF',
        critical: '#B23A2E',
        criticalTint: '#F7E3E0',
        high: '#B9791E',
        highTint: '#FBEBD3',
        medium: '#2F5FA0',
        mediumTint: '#E3EAF5',
        low: '#3F7D5C',
        lowTint: '#E1EEE7',
      },
      fontFamily: {
        sans: ['"IBM Plex Sans"', 'system-ui', 'sans-serif'],
        mono: ['"IBM Plex Mono"', 'ui-monospace', 'monospace'],
      },
      borderRadius: {
        DEFAULT: '3px',
      },
      spacing: {
        '4.5': '1.125rem',
      },
    },
  },
  plugins: [],
};

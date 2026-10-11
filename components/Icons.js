export function Logo() {
  return (
    <svg width="26" height="26" viewBox="0 0 26 26" fill="none">
      <rect width="26" height="26" rx="5" fill="#0F5C6B" />
      <path d="M13 6.5V19.5M6.5 13H19.5" stroke="#DCEBEA" strokeWidth="2" strokeLinecap="round" />
    </svg>
  );
}

const paths = {
  queue: 'M2 4h12M2 8h9M2 12h6',
  checkin: null, // custom below
  cal: null,
  history: null,
  chart: 'M3 13.5V7M8 13.5V3M13 13.5V9.5',
  cfg: null,
  log: null,
  patient: null,
  info: null,
  bolt: 'M8.6 2 3.5 9.2h3.3L6.6 14 12.4 6.4H9.1L8.6 2Z',
};

export function Icon({ name, className = '', size = 16 }) {
  const common = { width: size, height: size, viewBox: '0 0 16 16', fill: 'none', className };
  switch (name) {
    case 'queue':
      return (
        <svg {...common}>
          <path d="M2 4h12M2 8h9M2 12h6" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
        </svg>
      );
    case 'checkin':
      return (
        <svg {...common}>
          <path d="M6 8.3l1.6 1.6L11 6" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" strokeLinejoin="round" />
          <rect x="2" y="2" width="12" height="12" rx="2" stroke="currentColor" strokeWidth="1.4" />
        </svg>
      );
    case 'cal':
      return (
        <svg {...common}>
          <rect x="2" y="3.2" width="12" height="10.6" rx="1.2" stroke="currentColor" strokeWidth="1.4" />
          <path d="M2 6.4h12M5.3 2v2.6M10.7 2v2.6" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
        </svg>
      );
    case 'chart':
      return (
        <svg {...common}>
          <path d="M3 13.5V7M8 13.5V3M13 13.5V9.5" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
        </svg>
      );
    case 'cfg':
      return (
        <svg {...common}>
          <circle cx="8" cy="8" r="2.2" stroke="currentColor" strokeWidth="1.4" />
          <path
            d="M8 2v2M8 12v2M2 8h2M12 8h2M3.8 3.8l1.4 1.4M10.8 10.8l1.4 1.4M12.2 3.8l-1.4 1.4M5.2 10.8l-1.4 1.4"
            stroke="currentColor"
            strokeWidth="1.3"
            strokeLinecap="round"
          />
        </svg>
      );
    case 'log':
      return (
        <svg {...common}>
          <path d="M3 3h10v10H3z" stroke="currentColor" strokeWidth="1.4" />
          <path d="M5.3 6h5.4M5.3 8.5h5.4M5.3 11h3" stroke="currentColor" strokeWidth="1.3" strokeLinecap="round" />
        </svg>
      );
    case 'patient':
      return (
        <svg {...common}>
          <circle cx="8" cy="5.2" r="2.6" stroke="currentColor" strokeWidth="1.4" />
          <path d="M2.5 14c0-2.8 2.5-4.5 5.5-4.5s5.5 1.7 5.5 4.5" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
        </svg>
      );
    case 'info':
      return (
        <svg {...common} viewBox="0 0 16 16">
          <circle cx="8" cy="8" r="6.3" stroke="currentColor" strokeWidth="1.3" />
          <path d="M8 7.2v4M8 5.3v.1" stroke="currentColor" strokeWidth="1.4" strokeLinecap="round" />
        </svg>
      );
    case 'bolt':
      return (
        <svg {...common}>
          <path d="M8.6 2 3.5 9.2h3.3L6.6 14 12.4 6.4H9.1L8.6 2Z" stroke="currentColor" strokeWidth="1.2" strokeLinejoin="round" />
        </svg>
      );
    default:
      return null;
  }
}

import './globals.css';
import { AppProvider } from '@/context/AppProvider';

export const metadata = {
  title: 'Smart Hospital — Queue & Ward Console',
  description: 'Dynamic patient priority and hospital queue management console.',
};

export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <head>
        <link rel="preconnect" href="https://fonts.googleapis.com" />
        <link
          href="https://fonts.googleapis.com/css2?family=IBM+Plex+Sans:wght@400;500;600;700&family=IBM+Plex+Mono:wght@400;500;600&display=swap"
          rel="stylesheet"
        />
      </head>
      <body className="font-sans text-sm text-ink bg-bg antialiased">
        <AppProvider>{children}</AppProvider>
      </body>
    </html>
  );
}

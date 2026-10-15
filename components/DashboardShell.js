'use client';

import Link from 'next/link';
import { usePathname, useRouter } from 'next/navigation';
import { Logo, Icon } from './Icons';
import { ROLE_LABEL, ROLE_WHO } from '@/lib/mockData';
import { initials } from '@/lib/engine';
import { useApp } from '@/context/AppProvider';

const NAV = {
  reception: [
    ['checkin', 'Check-in', '/reception/checkin'],
    ['queue', 'Queue', '/reception/queue'],
  ],
  doctor: [['queue', 'Priority queue', '/doctor/queue']],
  admin: [
    ['chart', 'Overview', '/admin/overview'],
    ['cfg', 'Scoring config', '/admin/config'],
    ['log', 'Audit log', '/admin/log'],
  ],
  patient: [['cal', 'My visit', '/patient']],
};

const TITLES = {
  '/reception/checkin': ['Check-in', 'Register a walk-in or confirm an appointment arrival'],
  '/reception/queue': ['Live queue', 'Current wait line across all departments'],
  '/doctor/queue': ['Priority queue', 'Ranked by effective score — urgency plus time waited'],
  '/admin/overview': ['Overview', 'Queue composition and waiting-time performance'],
  '/admin/config': ['Scoring configuration', 'Adjust the weighting used by the priority engine'],
  '/admin/log': ['Audit log', 'Recent check-ins, calls and completions'],
  '/patient': ['My visit', 'Appointment, token and estimated wait'],
};

export default function DashboardShell({ role, children }) {
  const pathname = usePathname();
  const router = useRouter();
  const navItems = NAV[role] || [];
  const [title, meta] = TITLES[pathname] || ['Dashboard', ''];
  const { backendOnline } = useApp();

  return (
    <div className="flex min-h-screen">
      <aside className="w-[236px] shrink-0 bg-brandDark text-[#DCEBEA] flex flex-col sticky top-0 h-screen">
        <div className="px-5 pt-[22px] pb-[18px] border-b border-white/10">
          <div className="flex items-center gap-2.5">
            <Logo />
            <div>
              <div className="font-semibold text-[15px] text-white tracking-tight">Smart Hospital</div>
              <div className="text-[11.5px] text-[#8FB4B0] mt-0.5">Queue &amp; Ward Console</div>
            </div>
          </div>
        </div>
        <div className="px-5 py-3.5 border-b border-white/10 text-[12.5px] text-[#9EC4C0]">
          Signed in as
          <span className="block text-white font-semibold text-[13.5px] mt-0.5">
            {ROLE_WHO[role]} · {ROLE_LABEL[role]}
          </span>
        </div>
        <nav className="flex-1 px-2.5 py-2.5 overflow-y-auto">
          {navItems.map(([key, label, href]) => {
            const active = pathname === href;
            return (
              <Link
                key={key}
                href={href}
                className={`flex items-center gap-2.5 px-2.5 py-2.5 rounded text-[13.5px] mb-0.5 border ${
                  active
                    ? 'bg-white/10 text-white border-white/15'
                    : 'text-[#B9D6D3] border-transparent hover:bg-white/5 hover:text-white'
                }`}
              >
                <Icon name={key} />
                <span>{label}</span>
              </Link>
            );
          })}
        </nav>
        <div className="px-5 py-4 border-t border-white/10">
          <button
            onClick={() => router.push('/')}
            className="w-full text-left border border-white/25 text-[#DCEBEA] px-2.5 py-2 rounded text-[12.5px] hover:bg-white/10"
          >
            Switch role
          </button>
        </div>
      </aside>

      <div className="flex-1 min-w-0 flex flex-col">
        <div className="bg-surface border-b border-border px-7 py-3.5 flex items-center justify-between sticky top-0 z-10">
          <div>
            <h1 className="text-[17px] font-semibold m-0">{title}</h1>
            <div className="text-[12.5px] text-inkMuted mt-0.5">{meta}</div>
          </div>
          <div className="flex items-center gap-3">
            <span
              className={`inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full text-[11px] font-medium ${
                backendOnline
                  ? 'bg-emerald-50 text-emerald-700 border border-emerald-200'
                  : 'bg-gray-100 text-gray-600 border border-gray-200'
              }`}
            >
              <span
                className={`w-1.5 h-1.5 rounded-full ${
                  backendOnline ? 'bg-emerald-500 animate-pulse' : 'bg-gray-400'
                }`}
              />
              {backendOnline ? 'C++ REST Engine: Online' : 'Client Simulation'}
            </span>
            <div className="text-[12.5px] text-inkMuted font-mono tnum">
              {new Date().toLocaleDateString([], { weekday: 'short', month: 'short', day: 'numeric' })}
            </div>
            <div className="w-[30px] h-[30px] rounded-full bg-brandTint text-brandDark border border-borderStrong flex items-center justify-center text-xs font-semibold">
              {initials(ROLE_WHO[role])}
            </div>
          </div>
        </div>
        <div className="px-7 py-6 pb-16 max-w-[1180px]">{children}</div>
      </div>
    </div>
  );
}

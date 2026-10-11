'use client';

import { useState } from 'react';
import { useRouter } from 'next/navigation';
import { Logo, Icon } from '@/components/Icons';
import { useApp } from '@/context/AppProvider';
import { ROLE_HOME } from '@/lib/mockData';
import { effectiveScore, fmtWait } from '@/lib/engine';

const ROLES = [
  { key: 'reception', label: 'Receptionist', sub: 'Check-in, appointments, walk-ins', icon: 'checkin' },
  { key: 'doctor', label: 'Doctor', sub: 'Priority queue and consultations', icon: 'patient' },
  { key: 'admin', label: 'Admin', sub: 'Configuration and analytics', icon: 'cfg' },
  { key: 'patient', label: 'Patient', sub: 'Appointment and queue status', icon: 'cal' },
];

export default function LoginPage() {
  const router = useRouter();
  const { queue, config } = useApp();
  const [selected, setSelected] = useState('reception');

  const waiting = queue.filter((p) => p.status === 'waiting');
  const longestWait = waiting.length ? Math.max(...waiting.map((p) => p.waitMinutes)) : 0;
  const avgScore = waiting.length ? Math.round(waiting.reduce((s, p) => s + effectiveScore(p, config), 0) / waiting.length) : 0;

  return (
    <div className="min-h-screen flex">
      <div className="hidden lg:flex w-[46%] bg-gradient-to-br from-brandDark via-[#0D5561] to-brand text-white px-[52px] py-14 flex-col justify-between">
        <div className="flex items-center gap-2.5">
          <Logo />
          <div>
            <div className="font-semibold text-[16px]">Smart Hospital</div>
            <div className="text-[12.5px] text-[#AEDAD5] mt-0.5">Queue &amp; Ward Console</div>
          </div>
        </div>
        <div>
          <h2 className="text-[28px] leading-[1.3] font-semibold max-w-[380px] mt-10 mb-3.5">
            The queue reorders itself around who needs the doctor most.
          </h2>
          <p className="text-[13.5px] text-[#CFE7E4] max-w-[360px] leading-relaxed">
            Urgency, vitals and waiting time feed a transparent scoring model — every position in the line can
            be explained to staff on request.
          </p>
          <div className="mt-10 border-t border-white/20 pt-4.5 space-y-1">
            <Row label="Patients waiting" value={waiting.length} />
            <Row label="Longest wait" value={fmtWait(longestWait)} />
            <Row label="Avg. urgency score" value={avgScore} />
          </div>
        </div>
        <div className="text-[11.5px] text-[#7FB0AA]">Demo build — synthetic data only, no patient records.</div>
      </div>

      <div className="flex-1 flex items-center justify-center px-10 py-10">
        <div className="w-full max-w-[380px]">
          <h1 className="text-xl font-semibold mb-1">Sign in</h1>
          <div className="text-[13px] text-inkMuted mb-6">Choose how you&rsquo;d like to view the console for this session.</div>

          <div className="flex flex-col border border-border rounded overflow-hidden mb-5">
            {ROLES.map((r) => (
              <label
                key={r.key}
                className={`relative flex items-center gap-3 px-3.5 py-3.5 border-b border-border last:border-none cursor-pointer ${
                  selected === r.key ? 'bg-brandTint' : 'bg-white'
                }`}
              >
                <input
                  type="radio"
                  name="role"
                  className="sr-only"
                  checked={selected === r.key}
                  onChange={() => setSelected(r.key)}
                />
                <div className="w-8 h-8 rounded bg-surfaceAlt text-brand flex items-center justify-center shrink-0">
                  <Icon name={r.icon} />
                </div>
                <div>
                  <b className="block text-[13.5px]">{r.label}</b>
                  <span className="text-[11.5px] text-inkMuted">{r.sub}</span>
                </div>
                <div
                  className={`ml-auto w-4 h-4 rounded-full border shrink-0 ${
                    selected === r.key ? 'bg-brand border-brand' : 'border-borderStrong'
                  }`}
                />
              </label>
            ))}
          </div>

          <div className="flex flex-col gap-1.5 mb-3.5">
            <label className="text-[12.5px] font-medium text-inkMuted">Staff ID / Email</label>
            <input readOnly value={`${selected}@smarthospital.demo`} className="border border-borderStrong rounded px-2.5 py-2 bg-white" />
          </div>
          <div className="flex flex-col gap-1.5 mb-4">
            <label className="text-[12.5px] font-medium text-inkMuted">Password</label>
            <input readOnly type="password" value="demo-mode" className="border border-borderStrong rounded px-2.5 py-2 bg-white" />
          </div>

          <button
            onClick={() => router.push(ROLE_HOME[selected])}
            className="w-full bg-brand hover:bg-brandDark text-white rounded px-3.5 py-2.5 font-medium"
          >
            Continue
          </button>
          <div className="text-[11.5px] text-inkFaint text-center mt-2.5">

          </div>
        </div>
      </div>
    </div>
  );
}

function Row({ label, value }) {
  return (
    <div className="flex justify-between text-xs text-[#BFE0DC] py-1">
      <span>{label}</span>
      <b className="text-white font-mono tnum">{value}</b>
    </div>
  );
}

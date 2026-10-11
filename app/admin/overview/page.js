'use client';

import DashboardShell from '@/components/DashboardShell';
import { useApp } from '@/context/AppProvider';
import { DEPARTMENTS, DOCTORS } from '@/lib/mockData';
import { effectiveScore, levelOf, levelLabel, priorityInversions, fmtWait } from '@/lib/engine';
import { Stat, Banner } from '@/components/Bits';
import { Icon } from '@/components/Icons';

const BAR_COLOR = { critical: 'bg-critical', high: 'bg-high', medium: 'bg-medium', low: 'bg-low' };

export default function AdminOverviewPage() {
  return (
    <DashboardShell role="admin">
      <OverviewBody />
    </DashboardShell>
  );
}

function OverviewBody() {
  const { queue, config } = useApp();
  const waiting = queue.filter((p) => p.status === 'waiting');
  const counts = { critical: 0, high: 0, medium: 0, low: 0 };
  waiting.forEach((p) => counts[levelOf(effectiveScore(p, config))]++);
  const max = Math.max(...Object.values(counts), 1);
  const avgWait = waiting.length ? Math.round(waiting.reduce((s, p) => s + p.waitMinutes, 0) / waiting.length) : 0;

  return (
    <div className="flex flex-col gap-4.5">
      <div className="grid grid-cols-4 gap-3.5">
        <Stat value={queue.length} label="Patients today" />
        <Stat value={fmtWait(avgWait)} label="Average wait" />
        <Stat value={DOCTORS.length} label="Doctors on duty" />
        <Stat value={priorityInversions(waiting, config)} label="Priority inversions avoided" />
      </div>

      <div className="grid grid-cols-2 gap-4.5">
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Queue composition</h2>
          </div>
          <div className="p-4.5">
            <div className="flex items-end gap-4 h-[150px] pt-2.5">
              {['critical', 'high', 'medium', 'low'].map((l) => (
                <div key={l} className="flex-1 flex flex-col items-center justify-end h-full">
                  <div className="text-xs font-semibold mb-1 font-mono tnum">{counts[l]}</div>
                  <div className={`w-full rounded-t ${BAR_COLOR[l]}`} style={{ height: `${(counts[l] / max) * 100}%` }} />
                  <div className="text-[11px] text-inkMuted mt-1.5">{levelLabel(l)}</div>
                </div>
              ))}
            </div>
          </div>
        </div>
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Department load</h2>
          </div>
          <div>
            {DEPARTMENTS.map((d) => {
              const c = queue.filter((p) => p.dept === d && p.status !== 'completed').length;
              return (
                <div key={d} className="flex justify-between px-4.5 py-2.5 border-b border-border last:border-none text-[13px]">
                  <span>{d}</span>
                  <span className="font-mono tnum">{c}</span>
                </div>
              );
            })}
          </div>
        </div>
      </div>

      <Banner>
        <Icon name="info" className="text-brand shrink-0 mt-px" />
        <span>
          Figures are generated from the current in-memory demo queue and reset on reload. In the full build these
          come from controlled FIFO-vs-smart simulations (PRD §17), not production claims.
        </span>
      </Banner>
    </div>
  );
}

'use client';

import DashboardShell from '@/components/DashboardShell';
import { useApp } from '@/context/AppProvider';

export default function AdminLogPage() {
  return (
    <DashboardShell role="admin">
      <LogBody />
    </DashboardShell>
  );
}

function LogBody() {
  const { auditLog } = useApp();
  return (
    <div className="border border-border rounded bg-surface">
      <div className="px-4.5 py-3.5 border-b border-border">
        <h2 className="text-[14.5px] font-semibold m-0">Audit trail</h2>
      </div>
      <table className="w-full border-collapse text-[13px]">
        <thead>
          <tr>
            <th className="text-left font-semibold text-inkMuted text-[11.5px] px-2.5 py-2 border-b border-border">Time</th>
            <th className="text-left font-semibold text-inkMuted text-[11.5px] px-2.5 py-2 border-b border-border">Actor</th>
            <th className="text-left font-semibold text-inkMuted text-[11.5px] px-2.5 py-2 border-b border-border">Action</th>
          </tr>
        </thead>
        <tbody>
          {auditLog.map((a, i) => (
            <tr key={i} className="border-b border-border last:border-none">
              <td className="px-2.5 py-2.5 font-mono tnum text-inkMuted">{a.time}</td>
              <td className="px-2.5 py-2.5">{a.who}</td>
              <td className="px-2.5 py-2.5">{a.action}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

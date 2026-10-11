'use client';

import DashboardShell from '@/components/DashboardShell';
import { useApp } from '@/context/AppProvider';
import { RISK_FACTORS } from '@/lib/mockData';
import { Pill } from '@/components/Bits';

export default function AdminConfigPage() {
  return (
    <DashboardShell role="admin">
      <ConfigBody />
    </DashboardShell>
  );
}

const ROWS = [
  { key: 'symptomMax', label: 'Symptom severity', max: 40 },
  { key: 'vitalMax', label: 'Vital sign risk', max: 40 },
  { key: 'painMax', label: 'Pain severity', max: 20 },
  { key: 'ageMax', label: 'Age / risk factor', max: 20 },
  { key: 'riskMax', label: 'Existing risk factors', max: 20 },
];
const WAIT_ROWS = [
  { key: 'waitInterval', label: 'Minutes per wait-time point', max: 30 },
  { key: 'waitCap', label: 'Max wait-time bonus', max: 20 },
];

function ConfigBody() {
  const { config, updateConfigKey } = useApp();

  const sample = { symptom: 20, vital: 15, pain: 6, age: 5, risk: ['cardiac'] };
  const riskPts = Math.min(sample.risk.reduce((s, k) => s + RISK_FACTORS.find((r) => r.key === k).pts, 0), config.riskMax);
  const sampleScore =
    Math.min(sample.symptom, config.symptomMax) +
    Math.min(sample.vital, config.vitalMax) +
    Math.min(sample.pain, config.painMax) +
    Math.min(sample.age, config.ageMax) +
    riskPts;

  return (
    <div className="grid grid-cols-2 gap-4.5">
      <div className="border border-border rounded bg-surface">
        <div className="px-4.5 py-3.5 border-b border-border">
          <h2 className="text-[14.5px] font-semibold m-0">Weight caps</h2>
          <div className="text-[12.5px] text-inkMuted mt-0.5">Maximum points each factor can contribute to the urgency score</div>
        </div>
        <div className="p-4.5">
          {ROWS.map((r) => (
            <ConfigRow key={r.key} row={r} config={config} updateConfigKey={updateConfigKey} />
          ))}
          <div className="h-px bg-border my-4.5" />
          {WAIT_ROWS.map((r) => (
            <ConfigRow key={r.key} row={r} config={config} updateConfigKey={updateConfigKey} />
          ))}
        </div>
      </div>

      <div className="flex flex-col gap-4.5">
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Priority bands</h2>
          </div>
          <div>
            <BandRow level="critical" range="80–100" />
            <BandRow level="high" range="60–79" />
            <BandRow level="medium" range="40–59" />
            <BandRow level="low" range="0–39" />
          </div>
        </div>
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Sample calculation</h2>
          </div>
          <div className="p-4.5">
            <SampleRow label="Symptom severity (20 raw)" value={Math.min(sample.symptom, config.symptomMax)} />
            <SampleRow label="Vital sign risk (15 raw)" value={Math.min(sample.vital, config.vitalMax)} />
            <SampleRow label="Pain severity" value={Math.min(sample.pain, config.painMax)} />
            <SampleRow label="Age / risk factor" value={Math.min(sample.age, config.ageMax)} />
            <SampleRow label="Cardiac history" value={`+${riskPts}`} />
            <div className="flex justify-between pt-2.5 mt-1.5 border-t border-border">
              <span className="font-semibold text-sm">Urgency score</span>
              <b className="font-mono tnum text-base">{sampleScore}</b>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

function ConfigRow({ row, config, updateConfigKey }) {
  return (
    <div className="grid grid-cols-[150px_1fr_46px] items-center gap-2.5 mb-3">
      <label className="text-[12.5px] text-inkMuted">{row.label}</label>
      <input
        type="range"
        min="0"
        max={row.max}
        value={config[row.key]}
        onChange={(e) => updateConfigKey(row.key, Number(e.target.value))}
        className="w-full"
      />
      <div className="font-mono tnum text-[12.5px] text-right">{config[row.key]}</div>
    </div>
  );
}

function BandRow({ level, range }) {
  return (
    <div className="flex justify-between items-center px-4.5 py-2.5 border-b border-border last:border-none">
      <Pill level={level} />
      <b className="font-mono tnum">{range}</b>
    </div>
  );
}

function SampleRow({ label, value }) {
  return (
    <div className="flex justify-between text-sm py-1.5 border-b border-dashed border-border last:border-none">
      <span className="text-inkMuted">{label}</span>
      <b className="font-mono tnum font-semibold">{value}</b>
    </div>
  );
}

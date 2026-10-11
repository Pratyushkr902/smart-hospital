import { effectiveScore, levelOf, fmtWait } from '@/lib/engine';
import { Pill, ScoreBar } from './Bits';
import { DOCTORS } from '@/lib/mockData';

export function QueueRow({ patient, config, action }) {
  const score = effectiveScore(patient, config);
  const level = levelOf(score);
  const doctor = DOCTORS.find((d) => d.id === patient.doctorId);
  return (
    <tr className="border-b border-border last:border-none hover:bg-surfaceAlt">
      <td className="px-2.5 py-2.5 font-mono tnum text-inkMuted">{patient.token}</td>
      <td className="px-2.5 py-2.5">
        <b>{patient.name}</b>
        <div className="text-[11.5px] text-inkMuted">
          {patient.age} · {patient.gender === 'F' ? 'Female' : 'Male'} · {patient.dept}
        </div>
      </td>
      <td className="px-2.5 py-2.5">
        <Pill level={level} />
      </td>
      <td className="px-2.5 py-2.5 w-32">
        <div className="font-mono tnum font-semibold mb-1">{score}</div>
        <ScoreBar score={score} level={level} />
      </td>
      <td className="px-2.5 py-2.5 font-mono tnum">{fmtWait(patient.waitMinutes)}</td>
      <td className="px-2.5 py-2.5">{doctor ? doctor.name : '—'}</td>
      <td className="px-2.5 py-2.5">
        <Pill>{patient.status === 'in-treatment' ? 'In consultation' : patient.status === 'waiting' ? 'Waiting' : 'Completed'}</Pill>
      </td>
      {action && <td className="px-2.5 py-2.5">{action(patient)}</td>}
    </tr>
  );
}

export function QueueTable({ patients, config, action }) {
  if (!patients.length) {
    return <div className="py-8 px-4 text-center text-inkMuted text-sm">No patients currently waiting.</div>;
  }
  return (
    <table className="w-full border-collapse text-[13px]">
      <thead>
        <tr>
          {['Token', 'Patient', 'Level', 'Effective score', 'Waiting', 'Doctor', 'Status', action ? '' : null]
            .filter((x) => x !== null)
            .map((h) => (
              <th key={h} className="text-left font-semibold text-inkMuted text-[11.5px] px-2.5 py-2 border-b border-border whitespace-nowrap">
                {h}
              </th>
            ))}
        </tr>
      </thead>
      <tbody>
        {patients.map((p) => (
          <QueueRow key={p.id} patient={p} config={config} action={action} />
        ))}
      </tbody>
    </table>
  );
}

export function ComparePanel({ smart, fifo, config }) {
  return (
    <div className="grid grid-cols-2 gap-4 p-3.5">
      <div>
        <div className="text-xs font-semibold text-inkMuted mb-2 px-1">FIFO baseline — arrival order</div>
        {fifo.map((p, i) => (
          <div key={p.id} className="flex items-center gap-2.5 px-2 py-2.5 border-b border-border text-[12.5px]">
            <span className="font-mono tnum text-inkFaint w-[18px]">{i + 1}</span>
            <span className="flex-1">{p.name}</span>
            <Pill level={levelOf(effectiveScore(p, config))} />
          </div>
        ))}
      </div>
      <div>
        <div className="text-xs font-semibold text-inkMuted mb-2 px-1">Smart queue — effective score order</div>
        {smart.map((p, i) => {
          const fifoPos = fifo.findIndex((x) => x.id === p.id);
          const moved = fifoPos - i;
          return (
            <div key={p.id} className="flex items-center gap-2.5 px-2 py-2.5 border-b border-border text-[12.5px]">
              <span className="font-mono tnum text-inkFaint w-[18px]">{i + 1}</span>
              <span className="flex-1">{p.name}</span>
              {moved !== 0 && (
                <span className={`font-mono text-[11px] ${moved > 0 ? 'text-low' : 'text-critical'}`}>
                  {moved > 0 ? '▲' : '▼'} {Math.abs(moved)}
                </span>
              )}
              <Pill level={levelOf(effectiveScore(p, config))} />
            </div>
          );
        })}
      </div>
    </div>
  );
}

import { levelLabel } from '@/lib/engine';

const LEVEL_CLASSES = {
  critical: 'bg-criticalTint text-critical',
  high: 'bg-highTint text-high',
  medium: 'bg-mediumTint text-medium',
  low: 'bg-lowTint text-low',
};

export function Pill({ level, children }) {
  if (level) {
    return (
      <span className={`inline-flex items-center gap-1.5 rounded-full px-2.5 py-0.5 text-[11.5px] font-semibold ${LEVEL_CLASSES[level]}`}>
        <span className="w-1.5 h-1.5 rounded-full bg-current" />
        {levelLabel(level)}
      </span>
    );
  }
  return (
    <span className="inline-flex items-center gap-1.5 rounded-full bg-surfaceAlt text-inkMuted px-2.5 py-0.5 text-[11.5px] font-semibold">
      {children}
    </span>
  );
}

export function Stat({ value, label }) {
  return (
    <div className="border border-border rounded bg-surface px-4 py-3.5">
      <div className="text-2xl font-semibold font-mono tnum">{value}</div>
      <div className="text-xs text-inkMuted mt-0.5">{label}</div>
    </div>
  );
}

const BAR_COLOR = {
  critical: 'bg-critical',
  high: 'bg-high',
  medium: 'bg-medium',
  low: 'bg-low',
};

export function ScoreBar({ score, level }) {
  return (
    <div className="h-1.5 bg-surfaceAlt rounded overflow-hidden w-full">
      <div className={`h-full rounded ${BAR_COLOR[level]}`} style={{ width: `${score}%` }} />
    </div>
  );
}

export function Banner({ children }) {
  return (
    <div className="flex gap-2.5 items-start border border-border bg-surfaceAlt rounded px-3.5 py-2.5 text-[12.5px] text-inkMuted">
      {children}
    </div>
  );
}

export function Steps({ items, currentIndex }) {
  return (
    <div className="flex items-center">
      {items.map((s, i) => {
        const done = i < currentIndex;
        const current = i === currentIndex;
        return (
          <div key={s} className="flex flex-col items-center flex-1 relative">
            <div
              className={`w-[22px] h-[22px] rounded-full border-2 flex items-center justify-center text-[11px] font-semibold z-10 bg-white
              ${done ? 'bg-brand border-brand text-white' : current ? 'border-brand text-brand' : 'border-borderStrong text-inkFaint'}`}
            >
              {done ? '✓' : i + 1}
            </div>
            <div className="text-[11px] text-inkMuted mt-1.5 text-center">{s}</div>
            {i < items.length - 1 && (
              <div className={`absolute top-[11px] left-1/2 w-full h-0.5 ${done ? 'bg-brand' : 'bg-borderStrong'}`} />
            )}
          </div>
        );
      })}
    </div>
  );
}

export function KV({ label, value }) {
  return (
    <div className="flex justify-between text-sm py-1.5 border-b border-dashed border-border last:border-none">
      <span className="text-inkMuted">{label}</span>
      <b className="font-mono tnum font-semibold">{value}</b>
    </div>
  );
}

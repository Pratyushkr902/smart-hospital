'use client';

import DashboardShell from '@/components/DashboardShell';
import QueueBody from '@/components/QueueBody';

export default function ReceptionQueuePage() {
  return (
    <DashboardShell role="reception">
      <QueueBody isDoctor={false} />
    </DashboardShell>
  );
}

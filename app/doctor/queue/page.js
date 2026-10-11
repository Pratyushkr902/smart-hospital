'use client';

import DashboardShell from '@/components/DashboardShell';
import QueueBody from '@/components/QueueBody';

export default function DoctorQueuePage() {
  return (
    <DashboardShell role="doctor">
      <QueueBody isDoctor />
    </DashboardShell>
  );
}

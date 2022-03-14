import java.util.concurrent.Semaphore;
import java.lang.InterruptedException;
import java.util.concurrent.TimeUnit;

class NatBoundedBuffer extends BoundedBuffer {

   // Initialise the protected buffer structure above.
   NatBoundedBuffer(int maxSize) {
      super(maxSize);
   }

   // Extract an element from buffer. If the attempted operation is
   // not possible immediately, the method call blocks until it is.
   synchronized Object get() {
      Object value;

      // Enter mutual exclusion

      // Wait until there is a full slot available.
      if (size == 0) {
         try {
            wait();
         } catch (InterruptedException e) {
            System.out.println("Exception : InterruptException occurred");
         } catch (IllegalMonitorStateException e) {
            System.out.println("Exception : IllegalMonitorStateException occurred");
         }
      }

      // Signal or broadcast that an empty slot is available (if needed)
      if (size == maxSize) {
         notify();
      }

      value = super.get();
      return value;
      // Leave mutual exclusion
   }

   // Insert an element into buffer. If the attempted operation is
   // not possible immedidately, the method call blocks until it is.
   synchronized boolean put(Object value) {

      // Enter mutual exclusion

      // Wait until there is a empty slot available.
      if (size == maxSize) {
         try {
            wait();
         } catch (InterruptedException e) {
            System.out.println("Exception : InterruptException occurred");
         } catch (IllegalMonitorStateException e) {
            System.out.println("Exception : IllegalMonitorStateException occurred");
         }
      }

      // Signal or broadcast that a full slot is available (if needed)
      if (size == 0) {
         notify();
      }

      // Leave mutual exclusion
      return super.put(value);
   }

   // Extract an element from buffer. If the attempted operation is not
   // possible immedidately, return NULL. Otherwise, return the element.
   synchronized Object remove() {

      // Enter mutual exclusion

      // Signal or broadcast that an empty slot is available (if needed)

      return super.get();

      // Leave mutual exclusion
   }

   // Insert an element into buffer. If the attempted operation is
   // not possible immedidately, return 0. Otherwise, return 1.
   synchronized boolean add(Object value) {
      boolean done;

      // Enter mutual exclusion

      // Signal or broadcast that a full slot is available (if needed)

      return super.put(value);

      // Leave mutual exclusion
   }

   // Extract an element from buffer. If the attempted operation is not
   // possible immedidately, the method call blocks until it is, but
   // waits no longer than the given deadline. Return the element if
   // successful. Otherwise, return NULL.
   synchronized Object poll(long deadline) {
      long timeout;

      // Enter mutual exclusion

      // Wait until a full slot is available but wait
      // no longer than the given deadline

      if (size == 0)
         return null;

      // Signal or broadcast that an full slot is available (if needed)

      return super.get();

      // Leave mutual exclusion
   }

   // Insert an element into buffer. If the attempted operation is not
   // possible immedidately, the method call blocks until it is, but
   // waits no longer than the given deadline. Return 0 if not
   // successful. Otherwise, return 1.
   synchronized boolean offer(Object value, long deadline) {
      long timeout;

      // Enter mutual exclusion

      // Wait until a empty slot is available but wait
      // no longer than the given deadline

      if (size == maxSize)
         return false;

      // Signal or broadcast that an empty slot is available (if needed)

      super.put(value);

      // Leave mutual exclusion
      return true;
   }
}

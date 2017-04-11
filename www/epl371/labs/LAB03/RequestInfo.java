import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

/**
 * A custom Writable implementation for Request information.
 *
 * This is simple Custom Writable, and does not implement Comparable or RawComparator
 */
public class RequestInfo implements Writable {

    // Request ID as a String
    private Text requestId;

    // Request Type
    private Text requestType;

    // request timestamp
    private LongWritable timestamp;

    // 3 constructors
    public RequestInfo() {
        this.requestId = new Text();
        this.requestType = new Text();
        this.timestamp = new LongWritable();
    }

    public RequestInfo(Text requestId, Text requestType, LongWritable timestamp) {
        this.requestId = requestId;
        this.requestType = requestType;
        this.timestamp = timestamp;
    }

    public RequestInfo(String requestId, String requestType, long timestamp) {
        this.requestId = new Text(requestId);
        this.requestType = new Text(requestType);
        this.timestamp = new LongWritable(timestamp);
    }

    // The following 2 methods must be defined
    @Override
   //It serializes object data into byte stream data
    public void write(DataOutput dataOutput) throws IOException {
        requestId.write(dataOutput);
        requestType.write(dataOutput);
        timestamp.write(dataOutput);
    }

    @Override
   //overriding default readFields method. 
   //It de-serializes the byte stream data  
    public void readFields(DataInput dataInput) throws IOException {
        requestId.readFields(dataInput);
        requestType.readFields(dataInput);
        timestamp.readFields(dataInput);
    }

    // methods for getting/setting object characteristics
    public Text getRequestId() {
        return requestId;
    }

    public Text getRequestType() {
        return requestType;
    }

    public LongWritable getTimestamp() {
        return timestamp;
    }

    public void setRequestId(Text requestId) {
        this.requestId = requestId;
    }

    public void setRequestType(Text requestType) {
        this.requestType = requestType;
    }

    public void setTimestamp(LongWritable timestamp) {
        this.timestamp = timestamp;
    }
}
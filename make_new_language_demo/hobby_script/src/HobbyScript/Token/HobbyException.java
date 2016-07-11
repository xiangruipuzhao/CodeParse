package HobbyScript.Token;

/**
 * 定义了Hobby的运行异常
 *
 * @author liufengkai
 *         Created by liufengkai on 16/7/11.
 */
public class HobbyException extends RuntimeException {
    public HobbyException(String msg) {
        super(msg);
    }

//    public HobbyException(String msg, ASTree tree) {
//        super(msg + " " + tree.location());
//    }
}

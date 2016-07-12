package HobbyScript.Parser;

import java.util.Iterator;
import java.util.List;

/**
 * AST 抽象语法树的枝干
 *
 * @author liufengkai
 *         Created by liufengkai on 16/7/11.
 */
public class AstList extends AstNode {
    protected List<AstNode> children;

    public AstList(List<AstNode> children) {
        this.children = children;
    }

    @Override
    public AstNode child(int index) {
        return children.get(index);
    }

    @Override
    public Iterator<AstNode> children() {
        return children.iterator();
    }

    @Override
    public int childCount() {
        return children.size();
    }

    @Override
    public String location() {
        for (AstNode n : children) {
            String s = n.location();
            if (s != null) {
                return s;
            }
        }
        return null;
    }
}